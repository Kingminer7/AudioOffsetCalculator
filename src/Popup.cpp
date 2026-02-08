#include "Popup.hpp"
#include <Geode/ui/GeodeUI.hpp>

bool OffsetCalcPopup::init(CCTextInputNode* node) {
    if (!Popup::init(240, 160)) return false;
    setTitle("Audio Sync Calculator");

    m_input = node;

    m_startBtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("Start", 70, 0, 1.f, false, "goldFont.fnt", "GJ_button_01.png", 35.0), this, menu_selector(OffsetCalcPopup::onStart));
    m_startBtn->setID("start-btn");

    m_syncBtn = OffsetButton::create(ButtonSprite::create("Sync", 70, 0, 1.f, false, "goldFont.fnt", "GJ_button_02.png", 35.0), this, nullptr, this);
    m_syncBtn->setID("start-btn");
    m_syncBtn->setVisible(false);

    // @geode-ignore(unknown-setting)
    m_label = CCLabelBMFont::create(fmt::format("0/{}", Mod::get()->getSettingValue<int>("audio-cycles")).c_str(), "chatFont.fnt");
    m_mainLayer->addChildAtPosition(m_label, Anchor::Top, {0, -98});

    auto settingsSpr = CCSprite::createWithSpriteFrameName("accountBtn_settings_001.png");
    settingsSpr->setScale(.875f);
    auto settings = CCMenuItemSpriteExtra::create(settingsSpr, this, menu_selector(OffsetCalcPopup::onSettings));
    settings->setID("settings-btn");

    auto total = (m_startBtn->getScaledContentWidth() + settings->getScaledContentWidth() + 3) / 2;

    m_buttonMenu->addChildAtPosition(settings, Anchor::Center, {total - settings->getScaledContentWidth() / 2, -50});
    m_buttonMenu->addChildAtPosition(m_startBtn, Anchor::Center, {-total + m_startBtn->getScaledContentWidth() / 2, -50});
    m_buttonMenu->addChildAtPosition(m_syncBtn, Anchor::Center, {-total + m_startBtn->getScaledContentWidth() / 2, -50});

    auto lab =
        TextArea::create("Once you start, press the \"Sync\" button whenever the snare (second) sound plays.",
                         "chatFont.fnt", 1.f, 210.f, {0.5, 1}, 15.f, false);
    m_mainLayer->addChildAtPosition(lab, Anchor::Top, {0, -50});

    FMODAudioEngine::sharedEngine()->m_backgroundMusicChannel->setPaused(true);

    return true;
}

void OffsetCalcPopup::onSettings(CCObject* sender) {
    openSettingsPopup(Mod::get(), true);
}

void OffsetCalcPopup::onClose(CCObject* sender) {
    Popup::onClose(sender);
    FMODAudioEngine::sharedEngine()->m_backgroundMusicChannel->setPaused(false);
}

void OffsetCalcPopup::onPress(CCObject *sender) {
    m_presses.push_back(duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch())
        .count());
    m_label->setString(fmt::format("{}/{}", m_presses.size(), m_cycles).c_str());
    if (m_presses.size() >= m_cycles) {
        long offset = 0;
        for (size_t i = 0; i < m_presses.size(); i++) {
            offset += m_presses[i] - (m_startStamp + 1050 + 1500 * i);
        }
        offset = std::clamp(offset / (long) m_presses.size(), 0L, 10000L);
        m_input->setString(fmt::format("{}", offset).c_str());
        Notification::create(fmt::format("Set offset to {} ms.", offset).c_str(), NotificationIcon::Success)->show();
    }
}

void OffsetCalcPopup::onRelease(CCObject* sender) {
    if (m_presses.size() >= m_cycles) {
        m_syncBtn->setVisible(false);
        m_startBtn->setVisible(true);
    }
}

void OffsetCalcPopup::onStart(CCObject* sender) {
    m_startBtn->setVisible(false);
    m_syncBtn->setVisible(true);
    m_startStamp = duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                       .count();
    m_cycles = Mod::get()->getSettingValue<int>("audio-cycles");
    m_presses.clear();
    m_label->setString(fmt::format("{}/{}", m_presses.size(), m_cycles).c_str());
    auto sequence = CCSequence::create(
        CCCallFunc::create(this, callfunc_selector(OffsetCalcPopup::onKick)),
        CCDelayTime::create(0.75f),
        CCCallFunc::create(this, callfunc_selector(OffsetCalcPopup::onSnare)),
        CCDelayTime::create(0.75f),
        nullptr
    );
    auto repeatAction = CCRepeat::create(sequence, m_cycles);
    this->runAction(CCSequence::create(
        CCDelayTime::create(0.3f),
        repeatAction,
        nullptr
    ));
}

void OffsetCalcPopup::onKick() {
    m_current++;
    FMODAudioEngine::sharedEngine()->playEffect("kick.ogg"_spr);
}

void OffsetCalcPopup::onSnare() {
    FMODAudioEngine::sharedEngine()->playEffect("snare.ogg"_spr);
}

OffsetCalcPopup* OffsetCalcPopup::create(CCTextInputNode *node) {
    auto popup = new OffsetCalcPopup();
    if (popup && popup->init(node)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool OffsetButton::init(CCNode* node, CCObject* target, SEL_MenuHandler selector, OffsetCalcPopup* popup) {
    if (!CCMenuItemSpriteExtra::init(node, nullptr, target, selector)) {
        return false;
    }
    this->m_popup = popup;
    return true;
}

void OffsetButton::selected() {
    m_popup->onPress(this);
}

void OffsetButton::unselected() {
    m_popup->onRelease(this);
}

OffsetButton *OffsetButton::create(CCNode* node, CCObject* target, SEL_MenuHandler selector, OffsetCalcPopup* popup) {
    auto button = new OffsetButton();
    if (button && button->init(node, target, selector, popup)) {
        button->autorelease();
        return button;
    }
    delete button;
    return nullptr;
}
