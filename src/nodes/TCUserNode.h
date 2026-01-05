#ifndef TC_USER_NODE_H
#define TC_USER_NODE_H

#include <Geode/Geode.hpp>

class TCUserNode : public cocos2d::CCNode {
protected:
	cocos2d::CCSprite* m_userSprite{nullptr};
	cocos2d::CCLabelBMFont* m_userLabel{nullptr};
private:
	bool init(const gd::string& userName, const cocos2d::ccColor3B& color);
public:
	void setColor(const cocos2d::ccColor3B& color);

	static TCUserNode* create(const gd::string& userName, const cocos2d::ccColor3B& color);
	// static TCUserNode* createWithInfo();
};

#endif