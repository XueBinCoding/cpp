#ifndef EASYWSCLIENT_HPP_20120819_MIOFVASDTNUASZDQPLFD
#define EASYWSCLIENT_HPP_20120819_MIOFVASDTNUASZDQPLFD

#include <string>
#include <vector>

namespace easywsclient {


class WebSocket {
  public:
    typedef WebSocket * pointer;
    typedef enum readyStateValues { CLOSING, CLOSED, CONNECTING, OPEN } readyStateValues;

	//base64 encrypt
    static pointer from_url(const std::string& ip, const std::string& host, unsigned int port,const std::string& url, const std::string& origin = std::string());
    //no base64
	//static pointer from_url_no_mask(const std::string& ip, const std::string& host, unsigned int port, const std::string& url, const std::string& origin = std::string());

    // Interfaces:
    virtual ~WebSocket() { }
    virtual void poll(int timeout = 0) = 0; // timeout in milliseconds
    virtual void send(const std::string& message) = 0;
    virtual void sendBinary(const std::string& message) = 0;
    virtual void sendBinary(const std::vector<uint8_t>& message) = 0;
    virtual void sendPing() = 0;
    virtual void close() = 0;
    virtual readyStateValues getReadyState() const = 0;

	void getReceive(std::string& message) 
	{
		_getReceive(message);
	}

  protected:
	virtual void _getReceive(std::string& message) = 0;
};

} // namespace easywsclient

#endif /* EASYWSCLIENT_HPP_20120819_MIOFVASDTNUASZDQPLFD */
