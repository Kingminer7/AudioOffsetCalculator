#include "Popup.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/Geode.hpp>

bool OffsetCalcPopup::init(CCTextInputNode* node) {
    if (!Popup::init(240, 160)) return false;
    setTitle("Audio Sync Calculator");

    m_input = node;

    m_startBtn = Button::createWithNode(ButtonSprite::create("Start", 70, 0, 1.f, false, "goldFont.fnt", "GJ_button_01.png", 35.0), [this](auto) {
        m_startBtn->setVisible(false);
        m_syncBtn->setVisible(true);
        m_startStamp = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
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
        this->runAction(CCSequence::createWithTwoActions(
            CCDelayTime::create(0.3f),
            repeatAction
        ));
    });
    static_cast<CCNodeRGBA*>(m_startBtn->getDisplayNode())->setCascadeColorEnabled(true);
    m_startBtn->setID("start-btn");

    m_syncBtn = Button::createWithNode(ButtonSprite::create("Sync", 70, 0, 1.f, false, "goldFont.fnt", "GJ_button_02.png", 35.0));
    m_syncBtn->setSelectCallback([this](auto) {
        m_presses.push_back(duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
        m_label->setString(fmt::format("{}/{}", m_presses.size(), m_cycles).c_str());
        if (m_presses.size() >= m_cycles) {
            long offset = 0;
            for (size_t i = 0; i < m_presses.size(); i++) {
                offset += m_presses[i] - (m_startStamp + 1050 + 1500 * i);
            }
            offset = std::clamp(offset / (long) m_presses.size(), 0L, 10000L);
            m_input->setString(numToString(offset));
            Notification::create(fmt::format("Set offset to {} ms.", offset).c_str(), NotificationIcon::Success)->show();
            m_syncBtn->setVisible(false);
            m_startBtn->setVisible(true);
        }
    });
    static_cast<CCNodeRGBA*>(m_syncBtn->getDisplayNode())->setCascadeColorEnabled(true);
    m_syncBtn->setClickAnimation(CCSequence::createWithTwoActions(ColorTo::create(0, {150, 150, 150}), ColorTo::create(.3, {255, 255, 255})));
    m_syncBtn->setID("sync-btn");
    m_syncBtn->setVisible(false);

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

void OffsetCalcPopup::onKick() {
    m_current++;
    FMODAudioEngine::sharedEngine()->playEffect("kick.ogg"_spr);
}

void OffsetCalcPopup::onSnare() {
    FMODAudioEngine::sharedEngine()->playEffect("snare.ogg"_spr);
}

OffsetCalcPopup* OffsetCalcPopup::create(CCTextInputNode* node) {
    auto popup = new OffsetCalcPopup();
    if (popup->init(node)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

ColorTo* ColorTo::create(float duration, ccColor3B color) {
    auto ret = new ColorTo();
    if (!ret->initWithDuration(duration, color)) {
        delete ret;
        return nullptr;
    }
    ret->autorelease();
    return ret;
}

bool ColorTo::initWithDuration(float duration, ccColor3B color) {
    if (!CCActionInterval::initWithDuration(duration)) return false;
    m_endColor = color;
    return true;
}

static constexpr ccColor3B operator+(cocos2d::ccColor3B col, const cocos2d::ccColor3B& add) {
    col.r += add.r;
    col.g += add.g;
    col.b += add.b;
    return col;
}

static constexpr ccColor3B operator-(cocos2d::ccColor3B col, const cocos2d::ccColor3B& sub) {
    col.r -= sub.r;
    col.g -= sub.g;
    col.b -= sub.b;
    return col;
}

static constexpr ccColor3B operator*(cocos2d::ccColor3B col, const float mult) {
    col.r *= mult;
    col.g *= mult;
    col.b *= mult;
    return col;
}

void ColorTo::startWithTarget(CCNode* pTarget) {
    CCActionInterval::startWithTarget(pTarget);
    auto rgba = static_cast<CCNodeRGBA*>(pTarget);
    if (!rgba) return;
    m_startColor = rgba->getColor();
    m_deltaColor = m_endColor - m_startColor;
}



void ColorTo::update(float time) {
    if (!m_pTarget) return;
    auto rgba = static_cast<CCNodeRGBA*>(m_pTarget);
    if (!rgba) return;
    rgba->setColor(m_startColor + m_deltaColor * time);
}