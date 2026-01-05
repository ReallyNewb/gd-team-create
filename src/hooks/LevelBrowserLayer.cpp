#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

#include "../layers/TCUserColor.h"

namespace {
auto gam = GJAccountManager::get();
}

class $modify(TCBrowserLayer, LevelBrowserLayer) {
	bool init(GJSearchObject* object) {
		if (!LevelBrowserLayer::init(object)) return false;

		if (object->m_searchType == SearchType::MyLevels) {
			auto myLevelsMenu = this->getChildByID("my-levels-menu");

			auto userColorSprite = geode::CircleButtonSprite::create(
				cocos2d::CCSprite::createWithSpriteFrameName("GJ_paintBtn_001.png"),
				geode::CircleBaseColor::Pink, geode::CircleBaseSize::Medium
			);
			auto userColorBtn = CCMenuItemSpriteExtra::create(
				userColorSprite, this, menu_selector(TCBrowserLayer::onUserColor)
			);
			userColorBtn->setID("tc-user-color"_spr);

			myLevelsMenu->addChild(userColorBtn);
			myLevelsMenu->updateLayout();
		}

		return true;
	}

	void onUserColor(cocos2d::CCObject*) {
		if (gam->m_accountID > 0) TCUserColor::create()->show();
		else FLAlertLayer::create("No Account", "To use <cf>Team Create,</c> you need a <cg>Geometry Dash account.</c>\nPlease login to an <cy>account</c> before using this mod.", "OK")->show();
	}
};