#include "Popup.hpp"
#include <Geode/ui/GeodeUI.hpp>

bool OffsetCalcPopup::setup() {
    setTitle("Audio Sync Calculator");

    m_startBtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("Start", 70, 0, 1.f, false, "goldFont.fnt", "GJ_button_01.png", 35.0), this, menu_selector(OffsetCalcPopup::onStart));
    m_startBtn->setID("start-btn");

    m_syncBtn = OffsetButton::create(ButtonSprite::create("Sync", 70, 0, 1.f, false, "goldFont.fnt", "GJ_button_02.png", 35.0), this, nullptr, this);
    m_syncBtn->setID("start-btn");
    m_syncBtn->setVisible(false);

    // @geode-ignore(unknown-setting)
    m_label = CCLabelBMFont::create(fmt::format("0/{}", Mod::get()->getSettingValue<int>("audio-cycles")).c_str(), "chatFont.fnt");
    m_mainLayer->addChildAtPosition(m_label, Anchor::Top, {0, -98});
    m_buttonMenu->addChildAtPosition(m_startBtn, Anchor::Center, {0, -50});
    m_buttonMenu->addChildAtPosition(m_syncBtn, Anchor::Center, {0, -50});

    auto lab =
        TextArea::create("Once you start, press the \"Sync\" button whenever the snare (second) sound plays.",
                         "chatFont.fnt", 1.f, 210.f, {0.5, 1}, 15.f);
    m_mainLayer->addChildAtPosition(lab, Anchor::Top, {0, -50});
    GameSoundManager::sharedManager()->stopBackgroundMusic();

    return true;
}

void OffsetCalcPopup::onSettings(CCObject *sender) {
    openSettingsPopup(Mod::get(), true);
}

void OffsetCalcPopup::onClose(CCObject *sender) {
    Popup::onClose(sender);
    GameSoundManager::sharedManager()->playBackgroundMusic("menuLoop.mp3", true, false);
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
	    FMODAudioEngine::sharedEngine()->m_musicOffset = offset;
        Notification::create(fmt::format("Set offset to {} ms.", offset), NotificationIcon::Success)->show();
    }
}

void OffsetCalcPopup::onRelease(CCObject *sender) {
    if (m_presses.size() >= m_cycles) {
        m_syncBtn->setVisible(false);
        m_startBtn->setVisible(true);
    }
}

void OffsetCalcPopup::onStart(CCObject *sender) {
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
    GameSoundManager::sharedManager()->playEffect("kick.ogg"_spr, 1.f, 0.f, 1.f);
}

void OffsetCalcPopup::onSnare() {
    GameSoundManager::sharedManager()->playEffect("snare.ogg"_spr, 1.f, 0.f, 1.f);
}

OffsetCalcPopup *OffsetCalcPopup::create() {
    auto popup = new OffsetCalcPopup();
    if (popup && popup->initAnchored(240.f, 160.f)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool OffsetButton::init(CCNode *node, CCObject *target, SEL_MenuHandler selector, OffsetCalcPopup *popup) {
    if (!CCMenuItemSpriteExtra::initWithNormalSprite(node, nullptr, nullptr, target, selector)) {
        return false;
    }
    this->m_popup = popup;
    m_baseScale = this->getScale();
    return true;
}

void OffsetButton::selected() {
    m_popup->onPress(this);
}

void OffsetButton::unselected() {
    m_popup->onRelease(this);
}

OffsetButton *OffsetButton::create(CCNode *node, CCObject *target, SEL_MenuHandler selector, OffsetCalcPopup *popup) {
    auto button = new OffsetButton();
    if (button && button->init(node, target, selector, popup)) {
        button->autorelease();
        return button;
    }
    delete button;
    return nullptr;
}