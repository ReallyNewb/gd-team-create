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
		case 3:
			return "Please <cr>stop</c> trying to <cb>send requests</c> with <cl>bogus data.</c>";
		case 4: {
			geode::Mod::get()->setSavedValue<std::string>("argon-token", "");
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
		auto eToken = geode::Mod::get()->getSavedValue<std::string>("argon-token");
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
							geode::Mod::get()->setSavedValue<std::string>("argon-token", auth);

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

	void showAuthConsentPopup(const std::function<void(const std::string&, bool)>& callback) {
		auto eToken = geode::Mod::get()->getSavedValue<std::string>("argon-token");
		if (!eToken.empty()) {
			callback(eToken, true);
			return;
		}

		geode::MDPopup::create(
			"Authenticate With Argon",

			"# Argon\n"
			"Argon is an <cl>authentication method</c> created by the developers of <ca>Globed</c> and is trusted by the developer, <cy>RobTop.</c>\n\n"

			"Argon sends a <cg>message</c> to a bot account for verification. Argon <cr>**does and will NOT**</c>"
			" send your <cy>GD password</c> to a server for verification.\n\n"

			"You may read the privacy policy for specifics as to how your data is utilized.\n\n"

			"# Privacy Policy\n"

			"The data we collect are your <cl>username, accountID, userID, Argon token, icon colors, primary icon type and index, and user color.</c>\n"
			"The first 4 pieces of data are <cp>merely for Argon verification.</c> The rest are <ca>merely cosmetic</c> and <cr>won't share anything personal about your account.</c>\n\n"

			"To make support for the <cr>Friends Only</c> invite mode, we send your <cl>friends list</c> to the server.</c>\n\n"

			"The data we collect <cr>is NOT</c> shared with any <cb>third party services.</c> <cj>Level data</c> from our server are at times used for training a <cl>Zstd dictionary.</c> "
			"Note that <cl>Zstd dictionaries</c> are <co>entirely local</c> and don't rely on any external services.\n\n"

			"<cl>Zstd dictionaries</c> are merely for <cp>compressing</c> level data in <cy>**HUGE amounts**</c> "
			"to comply with <cf>disk space limits</c> from the host of the mod's server, [Spaceify.](https://spaceify.eu)\n\n"
			"By training <cl>Zstd dictionaries,</c> compressing can be made <cj>**MUCH faster.**</c>",

			"OK", "Authenticate",
			[callback](bool confirm) { if (confirm) startAuth(callback); }
		)->show();
	}
}

#endif