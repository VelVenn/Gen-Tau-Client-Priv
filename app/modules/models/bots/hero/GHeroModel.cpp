#include "bots/hero/GHeroModel.hpp"

#include "utils/TLog.hpp"

#include <algorithm>
#include <chrono>
#include <span>

#define T_LOG_TAG "[Hero Model] "

using namespace std::chrono_literals;

namespace gentau {
bool GHeroModel::isDeployVt() const noexcept
{
	return _isDeployVt;
}

bool GHeroModel::isDeployMode() const noexcept
{
	return _isDeployMode;
}

bool GHeroModel::isJPressed() const noexcept
{
	return _isJPressed;
}

bool GHeroModel::isHPressed() const noexcept
{
	return _isHPressed;
}

bool GHeroModel::isKPressed() const noexcept
{
	return _isKPressed;
}

bool GHeroModel::isLPressed() const noexcept
{
	return _isLPressed;
}

double GHeroModel::deployModeProgress() const noexcept
{
	return _deployModeProgress;
}

void GHeroModel::setDeployVt(bool isDeployVt)
{
	_deployVtQItem.setVisible(isDeployVt);
	_imgTransQItem.setVisible(!isDeployVt);

	if (_isDeployVt == isDeployVt) { return; }

	_isDeployVt = isDeployVt;
	Q_EMIT isDeployVtChanged(_isDeployVt);
}

void GHeroModel::setDeployMode(bool isDeployMode)
{
	if (_isDeployMode == isDeployMode) { return; }

	cancelDeployModePress();
	_isDeployMode = isDeployMode;

	if (_isDeployMode) { setDeployVt(true); }

	Q_EMIT isDeployModeChanged(_isDeployMode);
}

void GHeroModel::setJPressed(bool isPressed)
{
	if (_isJPressed == isPressed) { return; }

	_isJPressed = isPressed;
	Q_EMIT isJPressedChanged(_isJPressed);
}

void GHeroModel::setHPressed(bool isPressed)
{
	if (_isHPressed == isPressed) { return; }

	_isHPressed = isPressed;
	Q_EMIT isHPressedChanged(_isHPressed);
}

void GHeroModel::setKPressed(bool isPressed)
{
	if (_isKPressed == isPressed) { return; }

	_isKPressed = isPressed;
	Q_EMIT isKPressedChanged(_isKPressed);
}

void GHeroModel::setLPressed(bool isPressed)
{
	if (_isLPressed == isPressed) { return; }

	_isLPressed = isPressed;
	Q_EMIT isLPressedChanged(_isLPressed);
}

void GHeroModel::setDeployModeProgress(double progress)
{
	progress = std::clamp(progress, 0.0, 1.0);

	if (qFuzzyCompare(_deployModeProgress + 1.0, progress + 1.0)) { return; }

	_deployModeProgress = progress;
	Q_EMIT deployModeProgressChanged(_deployModeProgress);
}

bool GHeroModel::isHeroRanged()
{
	const auto status = commonStatus().staticStatus();

	return status.hasPerformanceSystemShooter() &&
		   status.performanceSystemShooter() ==
			   static_cast<quint32>(GBotCommonStatus::ShooterPerformance::HeroRanged);
}

void GHeroModel::startDeployModePress(bool targetMode)
{
	if (!isHeroRanged() || _isDeployMode == targetMode || _pendingDeployMode.has_value()) {
		return;
	}

	_pendingDeployMode = targetMode;
	setDeployModeProgress(0.0);
	_deployModeAnimation.start();
}

void GHeroModel::cancelDeployModePress()
{
	_deployModeAnimation.stop();
	_pendingDeployMode.reset();
	setDeployModeProgress(0.0);
}

void GHeroModel::publishDeployModeCommand(bool targetMode)
{
	HeroDeployModeEventCommand msg;
	msg.setMode(targetMode ? 1 : 0);

	const auto payload = msg.serialize(&_serializer);
	if (payload.isEmpty()) {
		tLogWarn(
			"Failed to serialize HeroDeployModeEventCommand: {}",
			_serializer.lastErrorString().toStdString()
		);
		return;
	}

	const auto result = publish("HeroDeployModeEventCommand", payload);
	if (!result.succeeded()) {
		tLogWarn("Failed to publish HeroDeployModeEventCommand: {}", result.cause.toStdString());
	}
}

void GHeroModel::updateEffectiveDeployMode()
{
	if (!isHeroRanged()) {
		cancelDeployModePress();
		setDeployMode(false);
		setDeployVt(false);
		return;
	}

	setDeployMode(_serverDeployMode);
}

void GHeroModel::parseDeployModeStatus(const QByteArray& data)
{
	DeployModeStatusSync msg;

	if (!msg.deserialize(&_serializer, data)) {
		tLogWarn(
			"Failed to parse DeployModeStatusSync: {}", _serializer.lastErrorString().toStdString()
		);
		return;
	}

	if (!msg.hasStatus()) {
		tLogWarn("Received DeployModeStatusSync without status");
		return;
	}

	_serverDeployMode = msg.status() == 1;
	updateEffectiveDeployMode();
}

void GHeroModel::parseCustomByteBlock(const QByteArray& data)
{
	CustomByteBlock msg;

	if (!msg.deserialize(&_serializer, data)) {
		static auto lastLogTime = std::chrono::steady_clock::now() - 5s;

		const auto now = std::chrono::steady_clock::now();
		if (now - lastLogTime >= 5s) {
			tLogWarn(
				"Failed to parse CustomByteBlock: {}", _serializer.lastErrorString().toStdString()
			);
			lastLogTime = now;
		}

		return;
	}

	if (!msg.hasData()) { return; }

	const auto                vidPkt = msg.data();
	const std::span<const u8> frame(
		reinterpret_cast<const u8*>(vidPkt.constData()), static_cast<size_t>(vidPkt.size())
	);

	(void)_deployVt.tryPushFrame(frame);
}

void GHeroModel::onNewKeyEvent(const GInputEventDispatcher::KeyboardEventInfo event)
{
	using KeyboardEventType = GInputEventDispatcher::KeyboardEventType;

	if (event.type == KeyboardEventType::Reset) {
		setJPressed(false);
		setHPressed(false);
		setKPressed(false);
		setLPressed(false);
		cancelDeployModePress();
		return;
	}

	const bool isPressed = event.type == KeyboardEventType::Press;

	switch (event.key) {
		case Qt::Key_J:
			if (_isJPressed == isPressed) { break; }
			setJPressed(isPressed);
			if (isPressed && !_isDeployMode) { setDeployVt(!_isDeployVt); }
			break;

		case Qt::Key_H:
			if (_isHPressed == isPressed) { break; }
			setHPressed(isPressed);
			if (isPressed && _isDeployVt) { restartDeployVt(); }
			break;

		case Qt::Key_K:
			if (_isKPressed == isPressed) { break; }
			setKPressed(isPressed);
			if (isPressed) {
				if (!_isDeployMode) { startDeployModePress(true); }
			} else if (_pendingDeployMode.has_value() && _pendingDeployMode.value()) {
				cancelDeployModePress();
			}
			break;

		case Qt::Key_L:
			if (_isLPressed == isPressed) { break; }
			setLPressed(isPressed);
			if (isPressed) {
				if (_isDeployMode) { startDeployModePress(false); }
			} else if (_pendingDeployMode.has_value() && !_pendingDeployMode.value()) {
				cancelDeployModePress();
			}
			break;

		default:
			break;
	}
}

void GHeroModel::restartDeployVt()
{
	if (!_isDeployVt) { return; }

	if (!_deployVt.flush()) { tLogWarn("Failed to flush deploy video renderer"); }

	if (!_deployVt.restart()) { tLogWarn("Failed to restart deploy video renderer"); }
}

void GHeroModel::resetStatus()
{
	_serverDeployMode = false;

	setJPressed(false);
	setHPressed(false);
	setKPressed(false);
	setLPressed(false);

	cancelDeployModePress();
	setDeployMode(false);
	setDeployVt(false);
}

GHeroModel::GHeroModel(InitPack deps) :
	GBotModel(deps.client, deps.curGen, deps.commonStatus),
	_deployVt(deps.deployVt),
	_inputCtrl(deps.inputCtrl),
	_imgTransQItem(deps.imgTransQItem),
	_deployVtQItem(deps.deployVtQItem)
{
	_deployModeAnimation.setDuration(static_cast<int>(pressDeployModeTimeOut));
	_deployModeAnimation.setStartValue(0.0);
	_deployModeAnimation.setEndValue(1.0);
	_deployModeAnimation.setEasingCurve(QEasingCurve::Linear);

	connect(
		&_deployModeAnimation,
		&QVariantAnimation::valueChanged,
		this,
		[this](const QVariant& value) { setDeployModeProgress(value.toDouble()); }
	);
	connect(&_deployModeAnimation, &QVariantAnimation::finished, this, [this] {
		const auto targetMode = _pendingDeployMode;
		_pendingDeployMode.reset();

		if (targetMode.has_value() && isHeroRanged() && _isDeployMode != targetMode.value()) {
			publishDeployModeCommand(targetMode.value());
		}

		setDeployModeProgress(0.0);
	});

	connect(
		&_inputCtrl, &GInputEventDispatcher::newKeyboardEvent, this, &GHeroModel::onNewKeyEvent
	);
	connect(
		&deps.commonStatus,
		&GBotCommonStatus::staticStatusChanged,
		this,
		[this](const RobotStaticStatus&) { updateEffectiveDeployMode(); }
	);

	setDeployVt(false);
	updateEffectiveDeployMode();

	const auto deployModeStatusResult = registerTopic(
		"DeployModeStatusSync", [this](const QByteArray& data) { parseDeployModeStatus(data); }
	);
	if (!deployModeStatusResult.succeeded()) {
		tLogError(
			"Failed to register DeployModeStatusSync: {}",
			deployModeStatusResult.cause.toStdString()
		);
		Q_ASSERT_X(false, "GHeroModel", "Failed to register DeployModeStatusSync");
	}

	const auto customByteBlockResult = registerTopic(
		"CustomByteBlock", [this](const QByteArray& data) { parseCustomByteBlock(data); }
	);
	if (!customByteBlockResult.succeeded()) {
		tLogError(
			"Failed to register CustomByteBlock: {}", customByteBlockResult.cause.toStdString()
		);
		Q_ASSERT_X(false, "GHeroModel", "Failed to register CustomByteBlock");
	}
}

GHeroModel::~GHeroModel()
{
	_deployModeAnimation.stop();
	_pendingDeployMode.reset();

	_deployVtQItem.setVisible(false);
	_imgTransQItem.setVisible(true);
}
}  // namespace gentau
