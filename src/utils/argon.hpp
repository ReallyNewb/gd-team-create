#ifndef NEWB_ARGON_HPP
#define NEWB_ARGON_HPP

#include <Geode/Geode.hpp>
#include <Geode/utils/coro.hpp>
#include <argon/argon.hpp>

namespace argonutils {
	geode::EventListener<geode::utils::web::WebTask> seshListener;

	std::string getPreErrorString(unsigned int errorCode, const matjson::Value& err) {
		switch (errorCode) {
		case 2:
			return "An <cr>error occurred</c> with <cf>verification</c> due to some error on <cl>Argon's end.</c> Error:";
		case 3: {
			geode::Mod::get()->setSavedValue<std::string>("authentication", "");
			return "Please <cr>stop</c> trying to <cb>send requests</c> with <cl>bogus data.</c> If this <cb>error appeared casually,</c> worry not, we have <cj>reset</c> your <cf>authentication token</c> <cr>**locally**</c> to <cg>attempt to prevent</c> any <ca>future issues similar to this.</c>\n\nMessage: ";
		}
		case 4: {
			geode::Mod::get()->setSavedValue<std::string>("authentication", "");
			return "Please <cg>refresh login</c> in <cr>account settings.</c>";
		}
		case 5: {
			geode::utils::clipboard::write(err.asString().unwrapOr(""));
			return "An <cg>Exception</c> was <cy>thrown.</c> Please report this <cr>error</c> to <cp>the mod's comments:</c>";
		}
		default: 
			return "Unknown error. Error:";
		}
	}

	std::string basePostString(const std::string& token) {
		auto gam = GJAccountManager::get();
		auto gm = GameManager::get();

		return fmt::format(
			"userName={}&userID={}&accountID={}&argon={}&pcolor={}&scolor={}&gcolor={}&genabled={}&icon={}&iconType={}",
			gam->m_username, gm->m_playerUserID, gam->m_accountID, token,
			gm->m_playerColor, gm->m_playerColor2, gm->m_playerGlowColor,
			(int) gm->m_playerGlow, gm->activeIconForType(gm->m_playerIconType), (int) gm->m_playerIconType
		);
	}

	void startAuth(const std::function<void(const std::string&, bool)>& callback) {
		auto eToken = geode::Mod::get()->getSavedValue<std::string>("authentication");
		if (!eToken.empty()) {
			callback(eToken, true);
			return;
		}

		$async(task = argon::startAuth(), callback) {
			auto res = co_await task;
			if (res.isOk()) {
				auto token = std::move(res).unwrap();

				auto web = geode::utils::web::WebRequest();
				web.bodyString(basePostString(token));

				argonutils::seshListener.bind([callback](geode::utils::web::WebTask::Event* e) {
					if (auto* res = e->getValue()) {
						if (res->ok()) {
							auto auth = res->json().unwrap()[0].asString().unwrap();
							geode::Mod::get()->setSavedValue<std::string>("authentication", auth);

							callback(auth, true);
							return;
						}

						callback("", false);
					}
					else if (e->getProgress()) return;
					else if (e->isCancelled()) return;
				});
				argonutils::seshListener.setFilter(web.post("http://192.168.1.46:80/argon"));
			} else {
				FLAlertLayer::create("Authentication Failed", fmt::format("Argon authentication <cr>failed.</c> The following error has already been copied to your clipboard:\n<cy>{}</c>", res.unwrapErr()).c_str(), "OK")->show();
				geode::utils::clipboard::write(res.unwrapErr());
				callback("", false);
			}
		};
	}

	void showAuthConsentPopup(const std::function<void(const std::string&, bool)>& callback, cocos2d::CCNode* show, cocos2d::CCNode* hide) {
		auto eToken = geode::Mod::get()->getSavedValue<std::string>("authentication");
		if (!eToken.empty()) {
			show->setVisible(true);
			hide->setVisible(false);
			callback(eToken, true);
			return;
		}

		geode::MDPopup::create(
			"Authenticate With Argon",

			"# About Argon\n"
			"Argon is an <cl>authentication method</c> created by the developers of <ca>Globed</c> and is <cg>trusted by the developer of Geometry Dash,</c> <cy>RobTop.</c>\n\n"

			"Argon sends a <cg>message</c> to a bot account for verification. Argon <cr>**does and will NOT**</c>"
			" send your <cy>GD password</c> to a server for verification.\n\n"

			"You may <cg>read</c> the <cr>**privacy policy**</c> for <ca>specifics</c> as to how <cj>your data is utilized.</c>\n\n"

			"# Privacy Policy\n"

			"## <cj>Data We Collect</c>\n"

			"The data we collect are your <cl>username, accountID, userID, Argon token, icon colors, primary icon type and index, and user color.</c>\n"
			"The first 4 pieces of data are <cp>merely for Argon verification.</c> The rest are <ca>merely cosmetic</c> and <cr>won't share anything personal about your account.</c>\n\n"

			"To make support for the <cr>Friends Only</c> invite mode, we send your <cl>friends list</c> to the server.</c>\n\n"

			"## <cj>Data Usage</c>\n"

			"<cj>Level data</c> from our server are at times used for training a <cl>Zstd dictionary.</c> "
			"Note that <cl>Zstd dictionaries</c> are <co>entirely local</c> and don't rely on any external services.\n\n"

			"<cl>Zstd dictionaries</c> are merely for <cp>compressing</c> level data in <cy>**HUGE amounts**</c> "
			"to comply with <cf>disk space limits</c> from the host of the mod's server, [Spaceify.](https://spaceify.eu)\n\n"
			"By training <cl>Zstd dictionaries,</c> compressing can be made <cj>**MUCH faster.**</c>\n\n"

			"### <cj>Data We (Never Will) Share</c>\n"

			"The data we collect <cr>is NOT</c> shared with any <cb>third party services.</c> The data is only kept in <cl>Team Create's database</c> and is <cr>**not distributed**</c> to <cb>other databases or random people.</c>\n"

			"## <cj>Data Security</c>\n"

			"We try to <cy>protect your data</c> by using some basic <cr>security methods</c> like <cl>verifying the data sent by the client</c> and <cj>parameterized queries</c> in <cb>SQL commands.</c>\n"
			"However, there may be <cr>security bugs</c> unnoticed, or <cp>I implemented a security method **really poorly.**</c>\n"
			"### <cy>If you ever find a security bug, PLEASE and DO send it to me (ReallyNewb) privately via the Geometry Dash messaging system.</c>\n"

			"## <cj>User Rights</c>\n"

			"<cg>Users</c> may <cr>manage their data</c> at anytime using the [mod's settings.](mod:reallynewb.team-create) If you have <co>any suggestion</c> for <cl>data management,</c>"
			" definitely <cy>post it</c> in the <cl>mod's comments.</c>\n"

			"## <cj>Data Retention</c>\n"

			"<cg>A user's data</c> is <cr>retained</c> as long as they <co>delete their account</c> via the [mod's settings.](mod:reallynewb.team-create) <cf>Deleting your account</c> will take up to <cy>14 days</c>"
			" <cb>just in case</c> you <cl>might've tapped too many buttons.</c>\n\nAs always, <cr>account deletion</c> is <ca>**ALWAYS revertable.**</c>\n"

			"## You may now <cf>go ahead</c> and <cj>start the authentication process.</c> If you <cr>don't feel like doing so right now,</c> <cy>consider authenticating later by pressing \"OK\".</c>",

			"OK", "Authenticate",
			[callback, show, hide](bool confirm) { 
				if (confirm) {
					show->setVisible(true);
					hide->setVisible(false);
					startAuth(callback); 
				}
			}
		)->show();
	}
}

#endif