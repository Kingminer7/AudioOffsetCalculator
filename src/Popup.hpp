#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Button.hpp>

using namespace geode::prelude;

class ColorTo : public cocos2d::CCActionInterval {
public:
    bool initWithDuration(float duration, ccColor3B color);
    virtual void startWithTarget(cocos2d::CCNode* pTarget);
    virtual void update(float time);
    static ColorTo* create(float duration, ccColor3B color);
protected:;
    ccColor3B m_startColor{};
    ccColor3B m_endColor{};
    ccColor3B m_deltaColor{};
};

class OffsetCalcPopup final : public Popup {
protected:
    Button* m_startBtn = nullptr;
    Button* m_syncBtn = nullptr;
    CCTextInputNode* m_input = nullptr;
    CCLabelBMFont* m_label = nullptr;
    long m_startStamp = 0;
    int m_current = 0;
    int m_cycles = 0;
    std::vector<long> m_presses;

    bool init(CCTextInputNode* node);
    void onKick();
    void onSnare();
public:
    static OffsetCalcPopup* create(CCTextInputNode* node);

    void onSettings(CCObject* sender);
    void onClose(CCObject* sender) override;
    void onStart(CCObject* sender);
};