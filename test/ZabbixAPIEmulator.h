#ifndef ZabbixAPIEmulator_h
#define ZabbixAPIEmulator_h

#include <glib.h>

class ZabbixAPIEmulator {
public:
	ZabbixAPIEmulator(void);
	virtual ~ZabbixAPIEmulator();

	bool isRunning(void);
	void start(guint port);

protected:
	static gpointer _mainThread(gpointer data);
	gpointer mainThread(void);

private:
	struct PrivateContext;
	PrivateContext *m_ctx;
};

#endif // ZabbixAPIEmulator_h
