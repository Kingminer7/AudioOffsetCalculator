#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/modify/MoreOptionsLayer.hpp>
#include "Popup.hpp"

using namespace geode::prelude;

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