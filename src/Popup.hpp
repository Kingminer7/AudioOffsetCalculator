#include <Geode/Geode.hpp>

using namespace geode::prelude;

class OffsetButton;

class OffsetCalcPopup : public geode::Popup<CCTextInputNode *> {
protected:
    CCMenuItemSpriteExtra *m_startBtn;
    OffsetButton *m_syncBtn;
    CCTextInputNode *m_input;
    CCLabelBMFont *m_label;
    long m_startStamp = 0;
    int m_current = 0;
    int m_cycles = 0;
    std::vector<long> m_presses;

    bool setup(CCTextInputNode *node) override;
    void onKick();
    void onSnare();
public:
    static OffsetCalcPopup *create(CCTextInputNode *node);

    void onSettings(CCObject *sender);
    void onClose(CCObject *sender) override;
    void onPress(CCObject *sender);
    void onRelease(CCObject *sender);
    void onStart(CCObject *sender);
};

class OffsetButton : public CCMenuItemSpriteExtra {
    protected:
        OffsetCalcPopup *m_popup;

        bool init(CCNode *node, CCObject *target, SEL_MenuHandler selector, OffsetCalcPopup *popup);

        void selected() override;
        void unselected() override;
    public:
        static OffsetButton *create(CCNode *node, CCObject *target, SEL_MenuHandler selector, OffsetCalcPopup *popup);
};