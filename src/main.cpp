#include <Geode/Geode.hpp>
#include "Popup.hpp"

#include <Geode/modify/CCLayer.hpp>

using namespace geode::prelude;

// legit just copied the button setting example from docs
class SyncSetting : public SettingV3 {
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<SyncSetting>();
        auto root = checkJson(json, "ButtonSetting");

        res->init(key, modID, root);
        res->parseNameAndDescription(root);
        res->parseEnableIf(root);
        
        root.checkUnknownKeys();
        return root.ok(std::static_pointer_cast<SettingV3>(res));
    }

    bool load(matjson::Value const& json) override {
        return true;
    }
    bool save(matjson::Value& json) const override {
        return true;
    }

    bool isDefaultValue() const override {
        return true;
    }
    void reset() override {}

    SettingNodeV3* createNode(float width) override;
};

class SyncSettingNode : public SettingNodeV3 {
protected:
    ButtonSprite* m_buttonSprite;
    CCMenuItemSpriteExtra* m_button;

    bool init(std::shared_ptr<SyncSetting> setting, float width) {
        if (!SettingNodeV3::init(setting, width))
            return false;
    
        
        m_buttonSprite = ButtonSprite::create("Sync", "goldFont.fnt", "GJ_button_04.png", .8f);
        m_buttonSprite->setScale(.75f);
        m_button = CCMenuItemSpriteExtra::create(
            m_buttonSprite, this, menu_selector(SyncSettingNode::onButton)
        );
        this->getButtonMenu()->addChildAtPosition(m_button, Anchor::Center);
        this->getButtonMenu()->setContentWidth(60);
        this->getButtonMenu()->updateLayout();

        this->updateState(nullptr);
        
        return true;
    }
    
    void updateState(CCNode* invoker) override {
        SettingNodeV3::updateState(invoker);

        auto shouldEnable = this->getSetting()->shouldEnable();
        m_button->setEnabled(shouldEnable);
        m_buttonSprite->setCascadeColorEnabled(true);
        m_buttonSprite->setCascadeOpacityEnabled(true);
        m_buttonSprite->setOpacity(shouldEnable ? 255 : 155);
        m_buttonSprite->setColor(shouldEnable ? ccWHITE : ccGRAY);
    }
    void onButton(CCObject*) {
        OffsetCalcPopup::create()->show();
    }

    void onCommit() override {}
    void onResetToDefault() override {}

public:
    static SyncSettingNode* create(std::shared_ptr<SyncSetting> setting, float width) {
        auto ret = new SyncSettingNode();
        if (ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool hasUncommittedChanges() const override {
        return false;
    }
    bool hasNonDefaultValue() const override {
        return false;
    }

    std::shared_ptr<SyncSetting> getSetting() const {
        return std::static_pointer_cast<SyncSetting>(SettingNodeV3::getSetting());
    }
};

SettingNodeV3* SyncSetting::createNode(float width) {
    return SyncSettingNode::create(
        std::static_pointer_cast<SyncSetting>(shared_from_this()),
        width
    );
}

$execute {
    (void)Mod::get()->registerCustomSettingType("button", &SyncSetting::parse);
}


class SongsSettingsPage : public CCLayer {};

// afaik alphas geode utils isn't ported to 1.9 so im doing it myself kinda
class $modify(SongsSettingsPageHook, CCLayer) {
    bool init() {
        if (!CCLayer::init()) return false;
        if (typeinfo_cast<SongsSettingsPage*>(this)) {
            queueInMainThread([this] {
                if (auto input = getChildByType<TextInput>(0)) {
                    auto spr = ButtonSprite::create("Sync", 0x46, 0, 0.6, false, "goldFont.fnt", "GJ_button_04.png", 25.0);
                    auto offsetCalc = Button::createWithNode(spr, [input](auto) {
                        if (!input) return FLAlertLayer::create("Error", "The audio offset input could not be found. Before reporting as a bug, check if you have another mod that changes the settings menu.", "Ok")->show();
                        OffsetCalcPopup::create(input->getInputNode())->show();
                    });
                    offsetCalc->setID("offset-calc");
                    offsetCalc->setPosition({-40, -105});
                    offsetCalc->setTouchPriority(-503);
                    input->addChildAtPosition(offsetCalc, Anchor::Left, {-35, 0});
                }
            });
        }
        return true;
    }
};