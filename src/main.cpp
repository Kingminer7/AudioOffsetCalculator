#include "ccTypes.h"
#include <Geode/modify/MoreOptionsLayer.hpp>
using namespace geode::prelude;

class OffsetCalcPopup : public geode::Popup<CCTextInputNode *> {
protected:
  CCMenuItemSpriteExtra *startBtn;
  CCMenuItemSpriteExtra *syncBtn;
  CCTextInputNode *node;
  bool setup(CCTextInputNode *node) override {
    this->node = node;
    // convenience function provided by Popup
    // for adding/setting a title to the popup
    setTitle("Audio Sync Calculator");

    startBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Start", 0x46, 0, 0.6f, false, "goldFont.fnt",
                             "GJ_button_01.png", 25.0),
        this, menu_selector(OffsetCalcPopup::onStart));
    startBtn->setID("start-btn");
    syncBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Sync", 0x46, 0, 0.6f, false, "goldFont.fnt",
                             "GJ_button_02.png", 25.0),
        this, menu_selector(OffsetCalcPopup::onPress));
    syncBtn->setID("sync-btn");
    syncBtn->setVisible(false);

    m_buttonMenu->addChildAtPosition(startBtn, Anchor::Center, {0, -40});
    m_buttonMenu->addChildAtPosition(syncBtn, Anchor::Center, {0, -40});

    auto lab =
        TextArea::create("Once you start, press the \"Sync\" button whenever "
                         "the snare sound plays.",
                         "chatFont.fnt", 1.f, 210.f, {0.5, 1}, 15.f, false);
    m_mainLayer->addChildAtPosition(lab, Anchor::Top, {0, -40});

    FMODAudioEngine::sharedEngine()->m_backgroundMusicChannel->setPaused(true);

    return true;
  }

  void onClose(CCObject *sender) override {
    Popup::onClose(sender);
    FMODAudioEngine::sharedEngine()->m_backgroundMusicChannel->setPaused(false);
  }

  long start = 0;
  int cycle = 0;
  long press1 = 0;
  long press2 = 0;
  long press3 = 0;
  long press4 = 0;

  void onPress(CCObject *) {
    auto now = duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();
    log::info("{} - {}", now, (start + 1500));
    if (press1 == 0) {
      syncBtn->setColor(ccColor3B{223, 255, 172});
      press1 = now - (start + 1180);
    } else if (press2 == 0) {
      syncBtn->setColor(ccColor3B{194, 253, 100});
      press2 = now - (start + 2680);
    } else if (press3 == 0) {
      syncBtn->setColor(ccColor3B{179, 255, 58});
      press3 = now - (start + 4180);
    } else if (press4 == 0) {
      syncBtn->setColor(ccColor3B{255, 255, 255});
      press4 = now - (start + 5680);
      syncBtn->setVisible(false);
      startBtn->setVisible(true);
      node->setString(
          fmt::format("{}", (press1 + press2 + press3 + press4) / 4));
    }
    log::info("{}, {}, {}, {}", press1, press2, press3, press4);
  }

  void onStart(CCObject *) {
    start = duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();
    cycle = 0;
    press1 = 0;
    press2 = 0;
    press3 = 0;
    press4 = 0;
    startBtn->setVisible(false);
    syncBtn->setVisible(true);
    scheduleOnce(schedule_selector(OffsetCalcPopup::onKick), 0.3);
  }

  void onKick(float) {
    cycle++;
    FMODAudioEngine::sharedEngine()->playEffect("kick.ogg"_spr);
    if (cycle > 4)
      return;
    scheduleOnce(schedule_selector(OffsetCalcPopup::onSnare), 0.75);
  }

  void onSnare(float) {
    FMODAudioEngine::sharedEngine()->playEffect("snare.ogg"_spr);
    if (cycle > 3)
      return;
    scheduleOnce(schedule_selector(OffsetCalcPopup::onKick), 0.75);
  }

public:
  static OffsetCalcPopup *create(CCTextInputNode *node) {
    auto ret = new OffsetCalcPopup();
    if (ret->initAnchored(240.f, 160.f, node)) {
      ret->autorelease();
      return ret;
    }

    delete ret;
    return nullptr;
  }
};

class $modify(CalcOptionsLayer, MoreOptionsLayer) {
  struct Fields {
    CCMenuItemSpriteExtra *offsetCalc;
  };

  bool init() {
    if (!MoreOptionsLayer::init()) {
      return false;
    }

    m_fields->offsetCalc = CCMenuItemSpriteExtra::create(
        // thanks robtop (i stole- "borrowed" your code)
        ButtonSprite::create("Sync", 0x46, 0, 0.6, false, "goldFont.fnt",
                             "GJ_button_04.png", 25.0),
        this, menu_selector(CalcOptionsLayer::onCalculateOffset));
    m_fields->offsetCalc->setID("offset-calc");
    m_fields->offsetCalc->setVisible(false);
    m_fields->offsetCalc->setPosition({-40, -105});

    getChildByIDRecursive("fmod-debug-button")->setPositionY(-105);

    m_buttonMenu->addChild(m_fields->offsetCalc);

    return true;
  }

  void goToPage(int page) {
    MoreOptionsLayer::goToPage(page);
    if (!m_fields->offsetCalc)
      return;
    if (page == 5) {
      m_fields->offsetCalc->setVisible(true);
    } else {
      m_fields->offsetCalc->setVisible(false);
    }
  }

  void onCalculateOffset(CCObject *) {
    OffsetCalcPopup::create(m_offsetInput)->show();
  }
};