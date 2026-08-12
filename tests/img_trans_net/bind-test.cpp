#include "img_trans/net/TRecv.hpp"
#include "img_trans/net/TReassembly.hpp"
#include "utils/TLog.hpp"

#include <atomic>
#include <csignal>
#include <exception>

#define T_LOG_TAG "[Bind Test] "

using namespace gentau;
using namespace std;

atomic_bool isRunning(false);

void onSignal(int signal)
{
    tLogInfo("Signal {} received, stopping TRecv...", signal);
    isRunning.store(false);
}

int main()
{
    try {
        TReassembly::SharedPtr reassembler = std::make_shared<TReassembly>(nullptr);
        auto                   recv        = TRecv::createUni(reassembler);

        signal(SIGINT, onSignal);
        signal(SIGTERM, onSignal);

        recv->start();

        tLogInfo("Start running ...");

        isRunning.store(true);

        while (isRunning.load()) {
            tLogDebug("Rebind now ...");
            recv->bindV4(0, "127.0.0.1");
            recv->start();

            this_thread::sleep_for(500ms);
        }
    } catch (const exception& ex) {
        tLogError("Error happend: {}", ex.what());
        return -1;
    }
}