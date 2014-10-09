#ifndef CARD_INFO_H
#define CARD_INFO_H
#include <string>

using namespace std;

namespace KVideo {
class CardInfo {
public:
	CardInfo();
	~CardInfo();
	string getIP();

	bool setIP(const string& strIP);

	int getPort();

	bool setPort(int port);

	string getUsername();

	bool setUsername(const string& username);

	string getPassword();

	bool setPassword(const string& password);

	string getCardDescription();

	bool setCardDescription(const string& description);

private:
	string m_ip;
	int m_port;
	string m_username;
	string m_password;
	string m_cardname;
};
}

#endif
