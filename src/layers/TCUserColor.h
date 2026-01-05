#ifndef TC_USER_COLOR_H
#define TC_USER_COLOR_H

#include <Geode/Geode.hpp>
#include "../nodes/TCUserNode.h"

class TCUserColor : public geode::Popup<>, public cocos2d::extension::ColorPickerDelegate, public TextInputDelegate {
protected:
	CCMenuItemSpriteExtra* m_submit{nullptr};
	geode::LoadingSpinner* m_loading{nullptr};

	geode::EventListener<geode::utils::web::WebTask> m_colorListener;
	cocos2d::extension::CCControlColourPicker* m_colorPicker{nullptr};
	TCUserNode* m_userNode{nullptr};

	geode::TextInput* m_r{nullptr};
	geode::TextInput* m_g{nullptr};
	geode::TextInput* m_b{nullptr};
private:
	bool setup() override;
	void onSubmit(cocos2d::CCObject*);
	
	void onPlayerColor(cocos2d::CCObject* sender);
	void onInfo(cocos2d::CCObject*);

	void colorValueChanged(cocos2d::ccColor3B color) override;
	void textChanged(CCTextInputNode* inputNode) override;
public:
	static TCUserColor* create();
};

#endif