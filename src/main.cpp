#include <minwindef.h>
#include <synchapi.h>
using namespace geode::prelude;

class OffsetCalcPopup : public geode::Popup<CCTextInputNode *> {
protected:
    bool setup(CCTextInputNode *node) override {
        // convenience function provided by Popup
        // for adding/setting a title to the popup
        setTitle("Audio Sync Calculator");

        auto startBtn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Start", 0x46, 0, 0.6f, false,"goldFont.fnt","GJ_button_01.png",25.0),
			this, 
			menu_selector(OffsetCalcPopup::onStart));
		startBtn->setID("start-btn");
		auto syncBtn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Sync", 0x46, 0, 0.6f, false,"goldFont.fnt","GJ_button_02.png",25.0),
			this, 
			menu_selector(OffsetCalcPopup::onPress));
		syncBtn->setID("sync-btn");
		syncBtn->setVisible(false);

		m_buttonMenu->addChildAtPosition(startBtn, Anchor::Center, {0,-40});
		m_buttonMenu->addChildAtPosition(syncBtn, Anchor::Center, {0,-40});

		auto lab = TextArea::create("Once you start, press the button whenever the snare sound plays.", "chatFont.fnt", 1.f, 220.f, {0.5, 1}, 15.f, false);
		m_mainLayer->addChildAtPosition(lab, Anchor::Top, {0, -40});

		FMODAudioEngine::sharedEngine()->m_backgroundMusicChannel->setPaused(true);

        return true;
    }

	void onClose(CCObject *sender) override {
		Popup::onClose(sender);
		FMODAudioEngine::sharedEngine()->m_backgroundMusicChannel->setPaused(false);
	}

	void onPress(CCObject *) {

	}

	int press1 = 0;
	int press2 = 0;
	int press3 = 0;
	int press4 = 0;

	void onStart(CCObject *) {
		FMODAudioEngine::sharedEngine()->playEffect("kick.ogg"_spr);
		Sleep(500);
		FMODAudioEngine::sharedEngine()->playEffect("snare.ogg"_spr);
		Sleep(500);
		FMODAudioEngine::sharedEngine()->playEffect("kick.ogg"_spr);
		Sleep(500);
		FMODAudioEngine::sharedEngine()->playEffect("snare.ogg"_spr);
		Sleep(500);
		FMODAudioEngine::sharedEngine()->playEffect("kick.ogg"_spr);
		Sleep(500);
		FMODAudioEngine::sharedEngine()->playEffect("snare.ogg"_spr);
		Sleep(500);
		FMODAudioEngine::sharedEngine()->playEffect("kick.ogg"_spr);
		Sleep(500);
		FMODAudioEngine::sharedEngine()->playEffect("snare.ogg"_spr);
	}

public:
    static OffsetCalcPopup* create(CCTextInputNode *node) {
        auto ret = new OffsetCalcPopup();
        if (ret->initAnchored(240.f, 160.f, node)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};

#include <Geode/modify/MoreOptionsLayer.hpp>
class $modify(CalcOptionsLayer, MoreOptionsLayer) {
	struct Fields {
        CCMenuItemSpriteExtra* offsetCalc;
    };

	bool init() {
		if (!MoreOptionsLayer::init()) {
			return false;
		}

		m_fields->offsetCalc = CCMenuItemSpriteExtra::create(
			// thanks robtop (i stole- "borrowed" your code)
			ButtonSprite::create("Sync",0x46,0,0.6,false,"goldFont.fnt","GJ_button_04.png",25.0),
			this,
			menu_selector(CalcOptionsLayer::onCalculateOffset)
		);
		m_fields->offsetCalc->setID("offset-calc");
		m_fields->offsetCalc->setVisible(false);
		m_fields->offsetCalc->setPosition({-40, -105});

		getChildByIDRecursive("fmod-debug-button")->setPositionY(-105);

		m_buttonMenu->addChild(m_fields->offsetCalc);

		return true;
	}

	void goToPage(int page) {
		MoreOptionsLayer::goToPage(page);
		if (!m_fields->offsetCalc) return;
		if (page == 5) {
			m_fields->offsetCalc->setVisible(true);
		} else {
			m_fields->offsetCalc->setVisible(false);
		}
	}

	void onCalculateOffset(CCObject*) {
		OffsetCalcPopup::create(m_offsetInput)->show();
	}
};