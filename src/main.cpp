#include "Geode/binding/CCTextInputNode.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/ui/Popup.hpp>

class OffsetCalcPopup : public geode::Popup<CCTextInputNode *> {
protected:
    bool setup(CCTextInputNode *node) override {
        // convenience function provided by Popup
        // for adding/setting a title to the popup
        this->setTitle("Hi mom!");

        auto label = CCLabelBMFont::create(value.c_str(), "bigFont.fnt");
        label->setPosition(this->m_mainLayer->getContentSize() / 2);
        this->m_mainLayer->addChild(label);

        return true;
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

using namespace geode::prelude;

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
			// thanks robtop
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