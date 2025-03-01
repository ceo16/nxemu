// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_common/logging/log.h"
#include "yuzu_common/scope_exit.h"
#include "yuzu_common/settings.h"
#include "yuzu_video_core/host1x/ffmpeg/ffmpeg.h"

extern "C" {
#ifdef LIBVA_FOUND
// for querying VAAPI driver information
#include <libavutil/hwcontext_vaapi.h>
#endif
}
