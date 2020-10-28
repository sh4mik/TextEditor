#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <windows.h>
#include <stdio.h>

#include "struct.h"
#include "write.h"
#include "menu.h"
#include "caret.h"
#include "save.h"

#include "debug.h"

// start parameters
#define FILENAME "very_small.txt"
#define MODE DEFAULT

// 1 - caret
// 7 - line
#define CARET_SIZE_COEFF 7

#define CARET_LENGTH 0.9

// GetStockObject(WHITE_BRUSH)
// COLOR_BACKGROUND
#define MY_BACK_COLOR GetStockObject(WHITE_BRUSH)

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */

int WINAPI WinMain (HINSTANCE hThisInstance,  // описатель самой программе
                     HINSTANCE hPrevInstance, // описатель последней программы, которая еще активна
                     LPSTR lpszArgument,  // параметры командной строки
                     int nCmdShow)  // каким должно вывестись окно (SW_SHOWNORMAL)
{
    HWND hwnd;          // Дескриптор главного окна программы
    MSG messages;       // Структура для хранения сообщения
    WNDCLASSEX wincl;   // Класс окна

    char hMenu[]="MainMenu";

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = "TextEdit";
    wincl.lpfnWndProc = WindowProcedure;    // Функция окна
    wincl.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;    // Стиль окна // TODO удалить лишнее
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Используем значок и указатель мыши по умолчанию */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);     // Стандартная иконка
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);   // Стандартная иконка
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);       // Стандартный курсор
    wincl.lpszMenuName = hMenu;                // Меню
    wincl.cbClsExtra = 0;                      // Нет дополнительных данных класса
    wincl.cbWndExtra = 0;                      // Нет дополнительных данных окна
    /* Используем цвет по умолчанию Windows в качестве фона окна. */
    wincl.hbrBackground = (HBRUSH) MY_BACK_COLOR;

    /* Регистрация класса окна */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (         // Имя класса окна
               0,                   // Дополнительые возможности
               "TextEdit",         // Имя класса
               ("TextEdit"),    // Заголовок окна
               WS_OVERLAPPEDWINDOW|WS_VSCROLL|WS_HSCROLL, // Стиль окна
               CW_USEDEFAULT,       // x
               CW_USEDEFAULT,       // y Размеры окна
               544,                 // Width
               375,                 // Height
               HWND_DESKTOP,        // Дескриптор родительского окна
               NULL,                // Нет меню
               hThisInstance,       // Дескриптор приложения
               lpszArgument         // Дополнительная информация
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

BOOL ShowDialog(HWND hwnd,TCHAR* sfile)
{

    OPENFILENAME ofn;

    ZeroMemory(&ofn, sizeof(ofn));
    ZeroMemory(sfile, sizeof(TCHAR)*MAX_PATH);
    ofn.lStructSize         = sizeof(ofn);
    ofn.hwndOwner           = NULL;
    ofn.lpstrFile           = sfile;
    ofn.nMaxFile            = MAX_PATH;
    ofn.lpstrFilter         = NULL;
    ofn.nFilterIndex        = 1;
    ofn.lpstrTitle          = TEXT("Please Select a File");
    ofn.lpstrInitialDir     = NULL;
    ofn.lpstrCustomFilter   = NULL;
    ofn.nMaxCustFilter      = 0;
    ofn.lpstrFileTitle      = NULL;
    ofn.nMaxFileTitle       = 0;
    ofn.nFileOffset         = 0;
    ofn.nFileExtension      = 0;
    ofn.lpstrDefExt         = NULL;
    ofn.lCustData           = 0;
    ofn.lpfnHook            = 0;
    ofn.lpTemplateName      = 0;
    ofn.Flags               = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    return GetOpenFileName(&ofn);
}

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    static TEXTMETRIC tm;
    static HFONT hFont;
    static HMENU hMenu;
    static int mode;
    static char file_name[MAX_PATH] = FILENAME;
    static data_t data = {NULL, 0, NULL, 0, 0}; // TODO структура может измениться
    static caret_t caret;
    static pos_t pos;
    static RECT rect;
    static rsize_t rSize;

    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            // извлекает дескриптор меню, связанного с заданным окном
            hMenu = GetMenu(hwnd);

            // создает логический шрифт с заданными характеристиками
            hFont = CreateFont(30, // Высота шрифта в логических единицах //TODO set 20
                            18,  // Ширина шрифта в логических единицах  //TODO set 12
                            0,  // Угол нанесения текста – угол между базовой линией текста и горизонталью (десятые доли градуса)
                            0,  // Наклон символов (десятые доли градуса)
                            FW_DONTCARE, // Один из флагов, определяющих жирность шрифта в пределах от 0 до 1000 (по умолчанию)
                            FALSE, // Ненулевое значение означает курсив
                            FALSE, // Ненулевое значение означает подчеркивание
                            FALSE, // Ненулевое значение означает перечеркнутый шрифт
                            DEFAULT_CHARSET, //Один из флагов, задающих кодировку шрифта (по умолчанию)
                            OUT_OUTLINE_PRECIS, // Один из флаго, определяющих соответствие запрашиваемого шрифта и имеющегося в наличии
                            CLIP_DEFAULT_PRECIS, // Один из флагов, определяющих способ “обрезания” изображения литер при их выходе за пределы области ограниченного вывода
                            DEFAULT_QUALITY, // Один из флагов, определяющих качество воспроизведения шрифта
                            VARIABLE_PITCH,TEXT("Lucida Console")); // Имя шрифта

            hdc = GetDC(hwnd);

            SelectObject(hdc,hFont); // устанавливает шрифт

            // заполняет заданный буфер метриками текущего выбранного шрифта
            GetTextMetrics(hdc,&tm);

            // извлекает координаты рабочей области окна
            GetClientRect(hwnd,&rect);

            // пишем размеры
            rSize.width = rect.right / tm.tmAveCharWidth;
            rSize.height = rect.bottom / tm.tmHeight;

            // зададим режим DEFAULT
            mode = MODE;
            CheckMenuItem(hMenu,MODE,MF_CHECKED);

            // создаем каретку
            CreateCaret(hwnd, NULL, tm.tmAveCharWidth / CARET_SIZE_COEFF, tm.tmHeight * CARET_LENGTH);


            FileRead(hwnd, FILENAME, &data); // TODO to delete
            FirstCountDefault(hwnd, &data);

            NewCaret(&data, &caret, tm);

            // Выводим с начала файла по умолчанию
            ZeroMemory(&pos, sizeof(pos));

            // Устанавливаем максимум скроллбара
            SetScrollRange(hwnd, SB_VERT, 0, SCROLL_CONST, TRUE);
            // Показываем каретку
            SetCaretPos(0, 0);
            ShowCaret(hwnd);


            return 0;

        case WM_SIZE: // отправляется окну после того, как изменился его размер
        {
            rSize.width = LOWORD(lParam) / tm.tmAveCharWidth; // функции сообщающие
            rSize.height = HIWORD(lParam) / tm.tmHeight; // размеры рабочей области

            SetScrollRange(hwnd, SB_HORZ, 0, fmax(1, data.longestSize - rSize.width), TRUE);

            // InvalidateRect(hwnd,NULL,TRUE);
            return 0;
        }
        case WM_VSCROLL: // отправляется в окно, когда событие прокрутки происходит в стандартной вертикальной полосе прокрутки окна
        {
            switch (LOWORD(wParam))
            {
                case SB_TOP :
                    pos.v = -5;
                break;

                case SB_BOTTOM :
                    pos.v = data.linesSize + 5;
                break;
                case SB_LINEUP: // Прокрутка на одну строку вверх
                    pos.v += -1;
                    break;
                case SB_LINEDOWN: // Прокрутка на одну строку вниз
                    pos.v += 1;
                    break;
                case SB_PAGEUP: // Прокрутка на одну страницу вверх
                    pos.v += - rSize.height;
                    break;
                case SB_PAGEDOWN: // Прокрутка на одну страницу вниз
                    pos.v += rSize.height;
                    break;
                case SB_THUMBTRACK: // Прокрутка до абсолютной позиции. Текущая позиция определяется старшим словом
                    pos.v = ScrollToPos(HIWORD(wParam), data, pos, rSize.height);
                    break;
                case SB_ENDSCROLL:
                    break;
                default:
                    pos.v = pos.v;
                    break;
            }
            // обыгрываем позиции в начале и конце файла

            pos.v = fmax(0, fmin(data.linesSize - rSize.height, pos.v));
            SetScrollPos(hwnd, SB_VERT, PosToScroll(pos.v, data, pos, rSize.height), TRUE);
            SetCaretPos((caret.sym - pos.h) * caret.w, (caret.line->number - pos.v) * caret.h);

            InvalidateRect(hwnd,NULL,TRUE);

            return 0;
        }
        case WM_HSCROLL: // отправляется в окно, когда событие прокрутки происходит в стандартной горизонтальный полосе прокрутки окна
        {
            switch (LOWORD(wParam))
            {
                case SB_LINEUP:
                    pos.h -= 1;
                    break;
                case SB_LINEDOWN:
                    pos.h += 1;
                    break;
                case SB_THUMBTRACK:
                    pos.h = fmax(0, HIWORD(wParam));
                    break;
                default:
                    pos.h += 0;
                    break;
            }
            pos.h = fmax(0,fmin(pos.h, data.longestSize - pos.h + 1));
            SetScrollPos(hwnd, SB_HORZ, pos.h, TRUE);

            SetCaretPos((caret.sym - pos.h) * caret.w, (caret.line->number - pos.v) * caret.h);
            // Сигнал на отрисовку
            InvalidateRect(hwnd,NULL,TRUE);
            return 0;
        }

        case WM_COMMAND: // отправляется тогда, когда пользователь выбирает командный пункт из меню
        {
            switch(LOWORD(wParam))
            {
                case ID_FILE_OPEN:
                {
                    free(&file_name);
                    if (!ShowDialog(hwnd,file_name)) //если диалог открылся
                        break;
                    freeData(&data);
                    ZeroMemory(&pos, sizeof(pos_t));

                    //если файл успешно считан и данные обработаны
                    if ((FileRead(hwnd, file_name, &data) == 1)){
                        DestroyCaret();
                        CreateCaret(hwnd, NULL, tm.tmAveCharWidth / CARET_SIZE_COEFF, tm.tmHeight * CARET_LENGTH);
                        pos.abs = data.text;

                        if (mode==DEFAULT) {
                            FirstCountDefault(hwnd, &data);
                            SetScrollRange(hwnd, SB_VERT, 0, SCROLL_CONST, FALSE);
                            SetScrollRange(hwnd, SB_HORZ, 0, fmax(1, data.longestSize - rSize.width), TRUE);

                            SetScrollPos(hwnd, SB_VERT, PosToScroll(pos.v, data, pos, rSize.height), TRUE);
                            SetScrollPos(hwnd, SB_HORZ, pos.h, TRUE);
                            InvalidateRect(hwnd,NULL,TRUE);
                        }

                        NewCaret(&data, &caret, tm);
                        SetCaretPos(0, 0);
                        ShowCaret(hwnd);
                    }
                    break;

                }
                case SAVE:
                {
                    SaveFile(hwnd, &data, &file_name);
                    break;
                }
                case IDM_EXIT:
                {
                    freeData(&data);
                    SendMessage(hwnd, WM_DESTROY, 0, 0L);
                    break;
                }
                default:
                    break;
            }
            return 0;
        }

        case WM_KEYDOWN: // ставится в очередь окна с фокусом клавиатуры тогда, когда нажата несистемная клавиша
        {
            switch (wParam)
            {
            case VK_UP:
                CaretUp(&caret, &data, &pos);
                //SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_LINEUP, 0), 0L);
                //SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_ENDSCROLL, 0), 0L);
                break;
            case VK_DOWN:
                CaretDown(&caret,  &data, &pos);
                //SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_LINEDOWN, 0), 0L);
                //SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_ENDSCROLL, 0), 0L);
                break;

            case VK_RIGHT:
                CaretRight(&caret, &data, &pos);
                //SendMessage(hwnd, WM_HSCROLL, MAKELONG(SB_LINERIGHT, 0), 0L);
                break;
            case VK_LEFT:
                CaretLeft(&caret, &data, &pos);
                //SendMessage(hwnd, WM_HSCROLL, MAKELONG(SB_LINELEFT, 0), 0L);
                break;
            case VK_PRIOR:
                // SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_PAGEUP, 0), 0L);
                CaretPageUp(&caret, &data, &pos, &rSize);
                break;
            case VK_NEXT:
                //SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_PAGEDOWN, 0), 0L);
                CaretPageDown(&caret, &data, &pos, &rSize);
                break;
            case VK_HOME:
                // SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_TOP, 0), 0L);
                CaretBegin(&caret, &data);
                break;
            case VK_END:
                //SendMessage(hwnd, WM_VSCROLL, MAKELONG(SB_BOTTOM, 0), 0L);
                CaretEnd(&caret, &data);
                break;
            default:
                break;
            }

            CaretDraw(&caret, &data, &pos, &tm, &rSize);
            // printf("%i ", pos.v);
            SetCaretPos((caret.sym - pos.h) * caret.w, (caret.line->number - pos.v) * caret.h);

            SetScrollPos(hwnd, SB_VERT, PosToScroll(pos.v, data, pos, rSize.height), TRUE);
            SetScrollPos(hwnd, SB_HORZ, pos.h, TRUE);
            //pos.abs = data.starts_of_lines[pos.v] - data.text;

            //printf("%i %i\n", caret.line->size, caret.sym);

            //SendMessage(hwnd, WM_VSCROLL, NULL, 0L);

            InvalidateRect(hwnd,NULL,TRUE);
            return 0;
        }

        case WM_CHAR:
            switch(wParam) {
                case '\b' :
                    DeleteSym(&data, &caret, &pos, mode);
                    CaretDraw(&caret, &data, &pos, &tm, &rSize);
                    SetCaretPos((caret.sym - pos.h) * caret.w, (caret.line->number - pos.v) * caret.h);

                    SetScrollPos(hwnd, SB_VERT, PosToScroll(pos.v, data, pos, rSize.height), TRUE);
                    SetScrollPos(hwnd, SB_HORZ, pos.h, TRUE);

                    //PrintBlocks(caret.line);
                    //PrintWindow(&data, &pos, rSize.height);

                    InvalidateRect(hwnd,NULL,TRUE);

                    break;


                case '\r' :
                    AddEnter(&data, &caret, &pos, mode, (char)wParam);
                    caret.sym = 0;
                    CaretDraw(&caret, &data, &pos, &tm, &rSize);
                    SetCaretPos((caret.sym - pos.h) * caret.w, (caret.line->number - pos.v) * caret.h);

                    SetScrollPos(hwnd, SB_VERT, PosToScroll(pos.v, data, pos, rSize.height), TRUE);
                    SetScrollPos(hwnd, SB_HORZ, pos.h, TRUE);

                    //PrintBlocks(caret.line);
                    //PrintWindow(&data, &pos, rSize.height);

                    InvalidateRect(hwnd,NULL,TRUE);

                    break;

                default:
                    AddSym(&data, &caret, &pos, mode, (char)wParam);
                    RecountAdd(&data, &caret, &pos, mode);
                    CaretDraw(&caret, &data, &pos, &tm, &rSize);
                    SetCaretPos((caret.sym - pos.h) * caret.w, (caret.line->number - pos.v) * caret.h);

                    SetScrollPos(hwnd, SB_VERT, PosToScroll(pos.v, data, pos, rSize.height), TRUE);
                    SetScrollPos(hwnd, SB_HORZ, pos.h, TRUE);

                    //PrintBlocks(caret.line);
                    //PrintWindow(&data, &pos, rSize.height);

                    InvalidateRect(hwnd,NULL,TRUE);

                    break;
            }
            // PrintWindow(&data, &pos, rSize.height);
            return 0;

        case WM_PAINT:
            ZeroMemory(&ps,sizeof(PAINTSTRUCT));
            hdc = BeginPaint(hwnd, &ps);
            MyTextOut(hdc, &data, &pos, &rSize, &tm);
            EndPaint(hwnd, &ps);
            return 0;



        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            free(&file_name);
            freeData(&data);
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
