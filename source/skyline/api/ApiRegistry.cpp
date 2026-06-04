#include "skyline/api/ApiRegistry.hpp"

#include <string>
#include <vector>

#include "nn/os.hpp"

namespace {

    struct Entry {
        std::string name;
        uint32_t version;
        const void* vtable;
    };

    struct Registry {
        nn::os::MutexType mutex;
        std::vector<Entry> entries;

        Registry() { nn::os::InitializeMutex(&mutex, false, 0); }
    };

    Registry& registry() {
        static Registry s_registry;
        return s_registry;
    }

    struct ScopedLock {
        nn::os::MutexType& m;
        explicit ScopedLock(nn::os::MutexType& mx) : m(mx) { nn::os::LockMutex(&m); }
        ~ScopedLock() { nn::os::UnlockMutex(&m); }
    };

}

extern "C" void sl_api_publish(const char* name, uint32_t version, const void* vtable) {
    if (name == nullptr)
        return;

    auto& reg = registry();
    ScopedLock lock(reg.mutex);

    for (auto& entry : reg.entries) {
        if (entry.version == version && entry.name == name) {
            entry.vtable = vtable;
            return;
        }
    }
    reg.entries.push_back(Entry{.name = name, .version = version, .vtable = vtable});
}

extern "C" const void* sl_api_get(const char* name, uint32_t version) {
    if (name == nullptr)
        return nullptr;

    auto& reg = registry();
    ScopedLock lock(reg.mutex);

    for (auto& entry : reg.entries) {
        if (entry.version == version && entry.name == name)
            return entry.vtable;
    }
    return nullptr;
}
