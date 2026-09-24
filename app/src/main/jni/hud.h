#pragma once

#include <algorithm>
#include <string>
#include <vector>

inline void RenderApollonHud() {
    if (!ImGui::GetCurrentContext() || screenWidth <= 0 || screenHeight <= 0) {
        return;
    }

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    if (viewport == NULL) {
        return;
    }

    const float scale = scaleGlobal > 0.0f ? scaleGlobal : 1.0f;
    const float padding = 12.0f * scale;
    const ImU32 panelColor = IM_COL32(12, 17, 26, 224);
    const ImU32 borderColor = IM_COL32(78, 96, 119, 190);
    const ImU32 accentColor = IM_COL32(92, 230, 207, 255);
    const ImU32 textColor = IM_COL32(244, 247, 250, 255);
    const ImU32 mutedColor = IM_COL32(160, 174, 191, 255);
    ImDrawList* drawList = ImGui::GetForegroundDrawList(viewport);
    const ImVec2 workPos = viewport->WorkPos;
    const ImVec2 workSize = viewport->WorkSize;

    // Compact client watermark in the upper-left corner.
    const ImVec2 watermarkMin(workPos.x + padding, workPos.y + padding);
    const ImVec2 watermarkMax(watermarkMin.x + 196.0f * scale,
                              watermarkMin.y + 44.0f * scale);
    drawList->AddRectFilled(watermarkMin, watermarkMax, panelColor,
                            8.0f * scale, ImDrawFlags_RoundCornersAll);
    drawList->AddRect(watermarkMin, watermarkMax, borderColor,
                      8.0f * scale, ImDrawFlags_RoundCornersAll,
                      1.0f * scale);
    drawList->AddRectFilled(
        watermarkMin,
        ImVec2(watermarkMin.x + 4.0f * scale, watermarkMax.y),
        accentColor, 8.0f * scale, ImDrawFlags_RoundCornersLeft);
    drawList->AddText(
        ImVec2(watermarkMin.x + 13.0f * scale,
               watermarkMin.y + 6.0f * scale),
        accentColor, "APOLLON");
    drawList->AddText(
        ImVec2(watermarkMin.x + 13.0f * scale,
               watermarkMin.y + 25.0f * scale),
        mutedColor, "CLIENT  |  v4.62  |  1.21.111");

    struct HudModuleState {
        const char* label;
        bool enabled;
    };

    const HudModuleState modules[] = {
        {"Always Sprint", alwaysSprinting},
        {"Anti Knockback", antiKnockback},
        {"KillAura", killAura},
        {"MobAura", mobAura},
        {"Auto Clicker", autoClicker},
        {"Step", step},
        {"Noclip", noclip},
        {"Fast Bridge", fastBridge},
        {"Block Reach", blockReach},
        {"Creative Fly", creativeFly},
        {"Force Creative Fly", forceCreativeFly},
        {"Damage Fly", damageFly},
        {"Strafe", strafe},
        {"FullBright", fullBright},
        {"Freelook", look360},
        {"ESP", esp},
        {"Xray", xrayDefault},
        {"Player Near", playerNear},
        {"Show Position", showPosition},
        {"Death Position", deathPosition},
        {"Hitbox", hitBox},
        {"No SlowDown", noSlowDown},
        {"No Hurt Cam", noHurtCam},
        {"No Blur", noBlur},
        {"Fast Fall", fastFall},
        {"Ladder Fly", ladderFly},
        {"Anti Blindness", antiBlind},
        {"Kill Near Items", killItems},
        {"Anti Fireball", antiFireball},
        {"Fast Drop", fastDrop},
        {"Place Camera", placeCam},
        {"No Boat Rotation", noBoatRotation},
    };

    std::vector<const char*> activeModules;
    activeModules.reserve(sizeof(modules) / sizeof(modules[0]));
    for (const HudModuleState& module : modules) {
        if (module.enabled) {
            activeModules.push_back(module.label);
        }
    }

    // Active features are arranged in two columns to keep the list readable.
    const float modulesWidth = 292.0f * scale;
    const float rowHeight = 18.0f * scale;
    const int rows = std::max(1, static_cast<int>((activeModules.size() + 1) / 2));
    const float modulesHeight = 34.0f * scale + rows * rowHeight;
    const ImVec2 modulesMin(workPos.x + workSize.x - modulesWidth - padding,
                            workPos.y + padding);
    const ImVec2 modulesMax(modulesMin.x + modulesWidth,
                            modulesMin.y + modulesHeight);
    drawList->AddRectFilled(modulesMin, modulesMax, panelColor,
                            8.0f * scale, ImDrawFlags_RoundCornersAll);
    drawList->AddRect(modulesMin, modulesMax, borderColor,
                      8.0f * scale, ImDrawFlags_RoundCornersAll,
                      1.0f * scale);
    drawList->AddText(
        ImVec2(modulesMin.x + 11.0f * scale,
               modulesMin.y + 8.0f * scale),
        accentColor, "ACTIVE FUNCTIONS");

    if (activeModules.empty()) {
        drawList->AddText(
            ImVec2(modulesMin.x + 11.0f * scale,
                   modulesMin.y + 25.0f * scale),
            mutedColor, "No active functions");
    } else {
        for (std::size_t i = 0; i < activeModules.size(); ++i) {
            const int row = static_cast<int>(i % rows);
            const int column = static_cast<int>(i / rows);
            const ImVec2 itemPos(
                modulesMin.x + (11.0f + column * 142.0f) * scale,
                modulesMin.y + (27.0f + row * 18.0f) * scale);
            drawList->AddCircleFilled(
                ImVec2(itemPos.x + 2.0f * scale,
                       itemPos.y + 6.0f * scale),
                2.2f * scale, accentColor);
            drawList->AddText(
                ImVec2(itemPos.x + 9.0f * scale, itemPos.y),
                textColor, activeModules[i]);
        }
    }

    // The target card appears only for a player who has just taken a hit.
    const PlayerStruct* hitTarget = NULL;
    for (const PlayerStruct& player : nearPlayerList) {
        if (player.hurtTime <= 0 || player.health <= 0 ||
            player.isBot || player.isFriend || player.isProtected) {
            continue;
        }
        if (hitTarget == NULL || player.hurtTime > hitTarget->hurtTime) {
            hitTarget = &player;
        }
    }

    const float targetWidth = 232.0f * scale;
    const float targetHeight = 88.0f * scale;
    const ImVec2 targetMin(workPos.x + workSize.x - targetWidth - padding,
                           workPos.y + workSize.y - targetHeight - padding);
    const ImVec2 targetMax(targetMin.x + targetWidth,
                           targetMin.y + targetHeight);
    drawList->AddRectFilled(targetMin, targetMax, panelColor,
                            8.0f * scale, ImDrawFlags_RoundCornersAll);
    drawList->AddRect(targetMin, targetMax, borderColor,
                      8.0f * scale, ImDrawFlags_RoundCornersAll,
                      1.0f * scale);
    drawList->AddText(
        ImVec2(targetMin.x + 11.0f * scale,
               targetMin.y + 8.0f * scale),
        accentColor, "TARGET");

    if (hitTarget == NULL) {
        drawList->AddText(
            ImVec2(targetMin.x + 11.0f * scale,
                   targetMin.y + 35.0f * scale),
            mutedColor, "Hit a player to show health");
    } else {
        const std::string& targetName =
            hitTarget->nameTag.empty() ? hitTarget->nickname
                                       : hitTarget->nameTag;
        char healthText[32];
        snprintf(healthText, sizeof(healthText), "HP  %d",
                 hitTarget->health);

        drawList->AddText(
            ImVec2(targetMin.x + 11.0f * scale,
                   targetMin.y + 29.0f * scale),
            textColor, targetName.c_str());
        drawList->AddText(
            ImVec2(targetMin.x + targetWidth - 65.0f * scale,
                   targetMin.y + 29.0f * scale),
            textColor, healthText);

        const ImVec2 barMin(targetMin.x + 11.0f * scale,
                            targetMin.y + 58.0f * scale);
        const ImVec2 barMax(targetMin.x + targetWidth - 11.0f * scale,
                            barMin.y + 8.0f * scale);
        const float healthFraction =
            std::max(0.0f, std::min(1.0f, hitTarget->health / 20.0f));
        const ImU32 healthColor = healthFraction > 0.5f
                                      ? IM_COL32(92, 230, 150, 255)
                                      : healthFraction > 0.25f
                                            ? IM_COL32(255, 190, 76, 255)
                                            : IM_COL32(255, 93, 105, 255);
        drawList->AddRectFilled(barMin, barMax,
                                IM_COL32(52, 62, 76, 255), 4.0f * scale);
        if (healthFraction > 0.0f) {
            drawList->AddRectFilled(
                barMin,
                ImVec2(barMin.x + (barMax.x - barMin.x) * healthFraction,
                       barMax.y),
                healthColor, 4.0f * scale);
        }
    }
}