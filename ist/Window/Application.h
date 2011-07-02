#ifndef __ist_Application__
#define __ist_Application__

#include <windows.h>

void glSwapBuffers();

namespace ist
{

struct WindowMessage
{
    enum TYPE
    {
        MES_CLOSE,
        MES_ACTIVE,
        MES_KEYBOARD,
        MES_MOUSE,
        MES_JOYSTICK,
        MES_RESIZE,
        MES_FOCUS,
    };

    int type;
};

struct WM_Close : public WindowMessage
{
};

struct WM_Active : public WindowMessage
{
    enum STATE
    {
        ST_ACTIVATED,
        ST_DEACTIVATED,
    };
    short state;
};

struct WM_Keyboard : public WindowMessage
{
    enum ACTION
    {
        ACT_KEYUP,
        ACT_KEYDOWN,
        ACT_CHAR,
    };
    enum KEY
    {
        KEY_ESCAPE = VK_ESCAPE,
    };

    short action;
    short key;
};

struct WM_Mouse : public WindowMessage
{
    enum ACTION
    {
        ACT_BUTTON_UP,
        ACT_BUTTON_DOWN,
        ACT_MOVE,
    };
    enum BUTTON
    {
        BU_LEFT     = 0x01,
        BU_RIGHT    = 0x02,
        BU_MIDDLE   = 0x10,
    };
    enum CONTROL
    {
        CT_CONTROL  = 0x08,
        CT_SHIFT    = 0x04,
    };

    short action;
    short button;
    short control;
    short x;
    short y;
};



class Application
{
private:
    HDC         m_hdc;
    HWND        m_hwnd;
    HGLRC       m_hglrc;
    DEVMODE     m_devmode;
    bool        m_fullscreen;

    size_t m_x, m_y;
    size_t m_width, m_height;

public:
    Application();
    virtual ~Application();

    virtual bool initialize(size_t x, size_t y, size_t width, size_t height, const wchar_t *title, bool fullscreen=false);
    virtual void finalize();

    virtual bool initializeDraw();
    virtual void finalizeDraw();

    virtual void mainLoop()=0;
    virtual int handleWindowMessage(const WindowMessage& wm)=0;

    void translateMessage();

    size_t getWindowWidth() const { return m_width; }
    size_t getWindowHeight() const { return m_height; }
};

} // namespace ist
#endif // __ist_Application__
