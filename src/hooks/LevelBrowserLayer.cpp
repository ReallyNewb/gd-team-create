#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

#include "../layers/TCUserColor.h"

namespace {
auto gam = GJAccountManager::get();
}

class $modify(TCBrowserLayer, LevelBrowserLayer) {
	struct Fields {
		bool m_inSharedLevels;
		cocos2d::CCSprite* m_sharedSprite{nullptr};
	};

	bool init(GJSearchObject* object) {
		if (!LevelBrowserLayer::init(object)) return false;

		if (object->m_searchType == SearchType::MyLevels) {
			auto myLevelsMenu = this->getChildByID("my-levels-menu");
			auto backMenu = this->getChildByID("back-menu");

			auto userColorSprite = geode::CircleButtonSprite::create(
				cocos2d::CCSprite::createWithSpriteFrameName("GJ_paintBtn_001.png"),
				geode::CircleBaseColor::Pink, geode::CircleBaseSize::Medium
			);
			auto userColorBtn = CCMenuItemSpriteExtra::create(
				userColorSprite, this, menu_selector(TCBrowserLayer::onUserColor)
			);
			userColorBtn->setID("tc-user-color"_spr);

			m_fields->m_sharedSprite = geode::CircleButtonSprite::create(
				cocos2d::CCSprite::createWithSpriteFrameName("GJ_sFriendsIcon_001.png"),
				geode::CircleBaseColor::Green, geode::CircleBaseSize::SmallAlt
			);
			auto sharedBtn = CCMenuItemSpriteExtra::create(
				m_fields->m_sharedSprite, this, menu_selector(TCBrowserLayer::onSharedLevels)
			);
			sharedBtn->setID("tc-shared-levels"_spr);

			myLevelsMenu->addChild(userColorBtn);
			myLevelsMenu->updateLayout();

			backMenu->addChild(sharedBtn);
			backMenu->updateLayout();
		}

		return true;
	}

	void onUserColor(cocos2d::CCObject*) {
		if (gam->m_accountID > 0) TCUserColor::create()->show();
		else FLAlertLayer::create("Account Required", "To use <cf>Team Create,</c> you need a <cg>Geometry Dash account.</c>\nPlease login to an <cy>account</c> before using this mod.", "OK")->show();
	}

	void onSharedLevels(cocos2d::CCObject*) {
		auto inShared = m_fields->m_inSharedLevels;

		auto sprFrame = cocos2d::CCSpriteFrameCache::get()->spriteFrameByName(inShared ? "geode.loader/baseCircle_SmallAlt_DarkPurple.png" : "geode.loader/baseCircle_SmallAlt_Green.png");
		m_fields->m_sharedSprite->setDisplayFrame(sprFrame);

		static_cast<cocos2d::CCLabelBMFont*>(m_list->getChildByID("title"))->setString(inShared ? "Shared With Me" : "My Levels");

		if (inShared) {
			auto testArray = cocos2d::CCArray::create();
			auto level = GJGameLevel::create();
			level->m_levelName = "Shared Testing";
			testArray->addObject(level);

			// idk
		}
	}
};