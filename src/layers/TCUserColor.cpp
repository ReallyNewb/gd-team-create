#include <Geode/Geode.hpp>
#include <cstdlib>

#include "../nodes/TCUserNode.h"
#include "../utils/argon.hpp"

#include "TCUserColor.h"

namespace {
auto gm = GameManager::get();
auto gam = GJAccountManager::get();
auto mod = geode::Mod::get();
}

bool TCUserColor::setup() {
	this->setTitle("Set User Color");

	auto size = m_mainLayer->getContentSize();
	m_colorPicker = cocos2d::extension::CCControlColourPicker::colourPicker();
	m_colorPicker->setPosition({size.width / 3.f, size.height / 2.f});
	m_colorPicker->setDelegate(this);

	auto userColor = mod->getSavedValue<cocos2d::ccColor3B>("user-color", {255, 255, 255});
	m_userNode = TCUserNode::create(gam->m_username, userColor);
	m_userNode->setPosition({size.width / 1.35f, size.height / 1.75f});

	m_r = geode::TextInput::create(50.f, "Red", "bigFont.fnt");
	m_g = geode::TextInput::create(50.f, "Green", "bigFont.fnt");
	m_b = geode::TextInput::create(50.f, "Blue", "bigFont.fnt");

	m_r->setCommonFilter(geode::CommonFilter::Uint);
	m_g->setCommonFilter(geode::CommonFilter::Uint);
	m_b->setCommonFilter(geode::CommonFilter::Uint);

	m_r->setPosition({size.width / 1.5f, size.height / 2.25f - 2.5f});
	m_g->setPosition({size.width * 0.815f, size.height / 2.25f - 2.5f});
	m_b->setPosition({size.width / 1.35f, size.height / 3.125f - 2.5f});

	m_r->setDelegate(this, 0);
	m_g->setDelegate(this, 1);
	m_b->setDelegate(this, 2);

	auto infoSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
	infoSpr->setScale(0.75f);
	auto info = CCMenuItemSpriteExtra::create(infoSpr, this, menu_selector(TCUserColor::onInfo));
	
	auto infoMenu = cocos2d::CCMenu::createWithItem(info);
	infoMenu->setPosition({size.width - 15.f, size.height - 15.f});

	auto pcButtons = cocos2d::CCMenu::create();

	auto c1B = CCMenuItemSpriteExtra::create(ButtonSprite::create("Primary Color"), this, menu_selector(TCUserColor::onPlayerColor));
	c1B->setTag(0);
	auto c2B = CCMenuItemSpriteExtra::create(ButtonSprite::create("Secondary Color"), this, menu_selector(TCUserColor::onPlayerColor));
	c2B->setTag(1);
	auto c3B = CCMenuItemSpriteExtra::create(ButtonSprite::create("Glow Color"), this, menu_selector(TCUserColor::onPlayerColor));
	c3B->setTag(2);
	c3B->setEnabled(gm->m_playerGlow);
	static_cast<cocos2d::CCSprite*>(c3B->m_pNormalImage)->setCascadeColorEnabled(true);
	c3B->setColor(gm->m_playerGlow ? (cocos2d::ccColor3B) {255, 255, 255} : (cocos2d::ccColor3B) {127, 127, 127});

	pcButtons->addChild(c1B);
	pcButtons->addChild(c2B);
	pcButtons->addChild(c3B);
	pcButtons->alignItemsHorizontallyWithPadding(10.f);
	pcButtons->setScale(0.5f);

	pcButtons->setPosition({size.width / 2.f, size.height / 1.8f});
	pcButtons->setAnchorPoint({0.f, 0.5f});

	m_submit = CCMenuItemSpriteExtra::create(ButtonSprite::create("Submit"), this, menu_selector(TCUserColor::onSubmit));
	m_loading = geode::LoadingSpinner::create(30.f);
	m_loading->setVisible(false);

	auto sMenu = cocos2d::CCMenu::createWithItem(m_submit);
	sMenu->setPosition({size.width / 2.f, size.height / 10.8f});
	m_loading->setPosition(sMenu->getPosition());

	m_mainLayer->addChild(m_colorPicker);
	m_mainLayer->addChild(m_userNode);

	m_mainLayer->addChild(m_r);
	m_mainLayer->addChild(m_g);
	m_mainLayer->addChild(m_b);

	m_mainLayer->addChild(infoMenu);
	m_mainLayer->addChild(pcButtons);

	m_mainLayer->addChild(sMenu);
	m_mainLayer->addChild(m_loading);

	m_colorPicker->setColorValue(userColor);

	return true;
}

void TCUserColor::onSubmit(cocos2d::CCObject*) {
	m_loading->setVisible(true);
	m_submit->setVisible(false);
	argonutils::showAuthConsentPopup([=, this](const std::string& token, bool success) {
		auto rgb = m_colorPicker->getColorValue();

		if (success) {
			auto web = geode::utils::web::WebRequest();
			web.bodyString(fmt::format("{}&r={}&g={}&b={}", argonutils::basePostString(token), rgb.r, rgb.g, rgb.b));

			m_colorListener.bind([this](geode::utils::web::WebTask::Event* e) {
				if (auto* res = e->getValue()) {
					if (res->ok()) {
						FLAlertLayer::create("Set User Color", "Set user color successfully.", "OK")->show();
					} 
					else {
						auto json = res->json().unwrapOr(matjson::Value::array());
						geode::MDPopup::create(
							"Failed To Set Color", 
							fmt::format(
								"{} <cy>{}</c>", 
								argonutils::getPreErrorString(json[1].asUInt().unwrapOr(0), json[0]), 
								json[0].asString().unwrapOr(res->string().unwrapOr(""))
							), 
							"OK", nullptr, nullptr
						)->show();
					}
					
					m_loading->setVisible(false);
					m_submit->setVisible(true);
				} 
				else if (e->getProgress()) return;
				else if (e->isCancelled()) return;
			});
			m_colorListener.setFilter(web.post("http://192.168.1.46:80/set-color"));
		} else {
			m_loading->setVisible(false);
			m_submit->setVisible(true);
		}

		mod->setSavedValue<cocos2d::ccColor3B>("user-color", rgb);
	});
}

void TCUserColor::onPlayerColor(cocos2d::CCObject* sender) {
	switch (sender->m_nTag) {
	case 0:
		m_colorPicker->setColorValue(gm->colorForIdx(gm->m_playerColor));
		break;
	case 1:
		m_colorPicker->setColorValue(gm->colorForIdx(gm->m_playerColor2));
		break;
	case 2:
		m_colorPicker->setColorValue(gm->colorForIdx(gm->m_playerGlowColor));
		break;
	}
}

void TCUserColor::onInfo(cocos2d::CCObject*) {
	FLAlertLayer::create("User Color Help", "Changes the <cr>color</c> of your <cl>user circle</c> for every <cf>Team Create session.</c>\n\nYou can modify your <cr>color</c> by using the <cp>color picker,</c> the <cy>color inputs,</c> or the <cj>player color buttons</c> at the top.", "OK")->show();
}

void TCUserColor::colorValueChanged(cocos2d::ccColor3B color) {
	m_userNode->setColor(color);

	m_r->setString(fmt::to_string(color.r), false);
	m_g->setString(fmt::to_string(color.g), false);
	m_b->setString(fmt::to_string(color.b), false);
}

void TCUserColor::textChanged(CCTextInputNode* inputNode) {
	auto value = m_colorPicker->m_rgb;
	const GLubyte val = static_cast<GLubyte>(std::min(255, atoi(inputNode->getString().c_str())));

	switch (inputNode->m_nTag) {
	case 0: 
		m_colorPicker->setColorValue({val, value.g, value.b});
		break;
	case 1:
		m_colorPicker->setColorValue({value.r, val, value.b});
		break;
	case 2:	
		m_colorPicker->setColorValue({value.r, value.g, val});
		break;
	}	
}

TCUserColor* TCUserColor::create() {
	auto size = cocos2d::CCDirector::get()->getWinSize();

	auto ret = new TCUserColor();

	if (ret->initAnchored(369.f, size.height - 50.f)) {
		ret->autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}