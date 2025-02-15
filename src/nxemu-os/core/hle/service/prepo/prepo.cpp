// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_common/hex_util.h"
#include "yuzu_common/logging/log.h"
#include "core/core.h"
#include "core/hle/service/acc/profile_manager.h"
#include "core/hle/service/ipc_helpers.h"
#include "core/hle/service/prepo/prepo.h"
#include "core/hle/service/server_manager.h"
#include "core/hle/service/service.h"

namespace Service::PlayReport {

class PlayReport final : public ServiceFramework<PlayReport> {
public:
    explicit PlayReport(const char* name, Core::System& system_) : ServiceFramework{system_, name} {
        // clang-format off
        static const FunctionInfo functions[] = {
            {10200, &PlayReport::RequestImmediateTransmission, "RequestImmediateTransmission"},
            {10300, &PlayReport::GetTransmissionStatus, "GetTransmissionStatus"},
            {10400, &PlayReport::GetSystemSessionId, "GetSystemSessionId"},
            {20100, &PlayReport::SaveSystemReport, "SaveSystemReport"},
            {20101, &PlayReport::SaveSystemReportWithUser, "SaveSystemReportWithUser"},
            {20200, nullptr, "SetOperationMode"},
            {30100, nullptr, "ClearStorage"},
            {30200, nullptr, "ClearStatistics"},
            {30300, nullptr, "GetStorageUsage"},
            {30400, nullptr, "GetStatistics"},
            {30401, nullptr, "GetThroughputHistory"},
            {30500, nullptr, "GetLastUploadError"},
            {30600, nullptr, "GetApplicationUploadSummary"},
            {40100, nullptr, "IsUserAgreementCheckEnabled"},
            {40101, nullptr, "SetUserAgreementCheckEnabled"},
            {50100, nullptr, "ReadAllApplicationReportFiles"},
            {90100, nullptr, "ReadAllReportFiles"},
            {90101, nullptr, "Unknown90101"},
            {90102, nullptr, "Unknown90102"},
            {90200, nullptr, "GetStatistics"},
            {90201, nullptr, "GetThroughputHistory"},
            {90300, nullptr, "GetLastUploadError"},
        };
        // clang-format on

        RegisterHandlers(functions);
    }

private:
    void RequestImmediateTransmission(HLERequestContext& ctx) {
        LOG_WARNING(Service_PREPO, "(STUBBED) called");

        IPC::ResponseBuilder rb{ctx, 2};
        rb.Push(ResultSuccess);
    }

    void GetTransmissionStatus(HLERequestContext& ctx) {
        LOG_WARNING(Service_PREPO, "(STUBBED) called");

        constexpr s32 status = 0;

        IPC::ResponseBuilder rb{ctx, 3};
        rb.Push(ResultSuccess);
        rb.Push(status);
    }

    void GetSystemSessionId(HLERequestContext& ctx) {
        LOG_WARNING(Service_PREPO, "(STUBBED) called");

        constexpr u64 system_session_id = 0;
        IPC::ResponseBuilder rb{ctx, 4};
        rb.Push(ResultSuccess);
        rb.Push(system_session_id);
    }

    void SaveSystemReport(HLERequestContext& ctx) {
        UNIMPLEMENTED();
    }

    void SaveSystemReportWithUser(HLERequestContext& ctx) {
        UNIMPLEMENTED();
    }
};

void LoopProcess(Core::System& system) {
    auto server_manager = std::make_unique<ServerManager>(system);

    server_manager->RegisterNamedService("prepo:a",
                                         std::make_shared<PlayReport>("prepo:a", system));
    server_manager->RegisterNamedService("prepo:a2",
                                         std::make_shared<PlayReport>("prepo:a2", system));
    server_manager->RegisterNamedService("prepo:m",
                                         std::make_shared<PlayReport>("prepo:m", system));
    server_manager->RegisterNamedService("prepo:s",
                                         std::make_shared<PlayReport>("prepo:s", system));
    server_manager->RegisterNamedService("prepo:u",
                                         std::make_shared<PlayReport>("prepo:u", system));
    ServerManager::RunServer(std::move(server_manager));
}

} // namespace Service::PlayReport
