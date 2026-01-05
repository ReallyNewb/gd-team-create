#pragma once
#include <Geode/Geode.hpp>

#include "TCUserNode.h"

bool TCUserNode::init(const gd::string& userName, const cocos2d::ccColor3B& color) {
	if (!cocos2d::CCNode::init()) return false;

	m_userSprite = cocos2d::CCSprite::createWithSpriteFrameName("explosionIcon_11_001.png");
	m_userSprite->setScale(1.25f);

	auto userNode = cocos2d::CCNode::create();
	m_userLabel = cocos2d::CCLabelBMFont::create(userName.c_str(), "chatFont.fnt");
	
	auto userLabelS = cocos2d::CCLabelBMFont::create(userName.c_str(), "chatFont.fnt");
	userLabelS->setColor({0, 0, 0});
	userLabelS->setPosition(0.5f, -0.5f);

	userNode->addChild(m_userLabel);
	userNode->addChild(userLabelS, -1);

	userNode->setPosition({0.f, 30.5f});
	userNode->setScale(1.25f);

	this->addChild(m_userSprite);
	this->addChild(userNode);

	this->setColor(color);

	return true;
}

void TCUserNode::setColor(const cocos2d::ccColor3B& color) {
	m_userSprite->setColor(color);
	m_userLabel->setColor(color);
}

TCUserNode* TCUserNode::create(const gd::string& userName, const cocos2d::ccColor3B& color) {
	auto ret = new TCUserNode();

	if (ret->init(userName, color)) {
		ret->autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}