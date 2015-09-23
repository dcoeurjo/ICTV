
#include <cassert>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstring>

#include "Widgets/nvWidgets.h"


namespace nv {

const Rect Rect::null;

UIContext::UIContext( UIFont *font, UIPainter *painter )
    :
    m_painter(painter),
    m_font(font),
    m_nbKeys(0),
    m_fileBuffer(NULL),
    m_focusCaretPos(-1),
    m_twoStepFocus(false)
{
    assert(painter != NULL);
    assert(m_font != NULL);
}


void UIContext::mouse(int button, int state, int modifier, int x, int y) {
    setCursor(x, y);

    int idx = -1;
    if (button == MouseButton_Left) idx = 0;
    else if (button == MouseButton_Middle) idx = 1;
    else if (button == MouseButton_Right) idx = 2;

    modifier &= ButtonFlags_Alt | ButtonFlags_Shift | ButtonFlags_Ctrl;

    if (idx >= 0) {
        if (state == 1) {
            m_mouseButton[idx].state = ButtonFlags_On | ButtonFlags_Begin | modifier;
            m_mouseButton[idx].time = clock();
            m_mouseButton[idx].cursor.x = x;
            m_mouseButton[idx].cursor.y = m_window.h - y;
        }
        if (state == 0) {
            m_mouseButton[idx].state = ButtonFlags_On | ButtonFlags_End | modifier;
        }
    }
}

void UIContext::mouseMotion(int x, int y) {
    setCursor(x, y);
}

void UIContext::keyboard(unsigned int k, int x, int y) {
    setCursor(x, y);
    assert(m_nbKeys < 1024);
    m_keyBuffer[m_nbKeys]= k;
    m_nbKeys++;
}

void UIContext::text(const char *string, int x, int y) {
    setCursor(x, y);
    for(unsigned int i= 0; string[i] != 0; i++) {
        assert(m_nbKeys < 1024);
        m_keyBuffer[m_nbKeys]= string[i];
        m_nbKeys++;
    }
}

void UIContext::file( const char *file, int x, int y ) {
    setCursor(x, y);
    if(m_fileBuffer != NULL) free(m_fileBuffer);
    m_fileBuffer= strdup(file);
}


void UIContext::reshape(int w, int h) {
    reshape(0, 0, w, h);
}

void UIContext::reshape(int x, int y, int w, int h) {
    m_window.x = x;
    m_window.y = y;
    m_window.w = w;
    m_window.h = h;
    
    m_painter->reshape(m_window);
}

void UIContext::begin() {
    m_painter->begin();

    m_groupIndex = 0;
    m_groupStack[m_groupIndex].flags = GroupFlags_LayoutNone;
    m_groupStack[m_groupIndex].margin = m_painter->getCanvasMargin();
    m_groupStack[m_groupIndex].space = m_painter->getCanvasSpace();
    m_groupStack[m_groupIndex].bounds = m_window;
}

void UIContext::end() {
    m_painter->end();

    // Release focus.
    if (m_mouseButton[0].state & ButtonFlags_End) m_uiOnFocus = false;

    // Update state for next frame.
    for (int i = 0; i < 3; i++) {
        if (m_mouseButton[i].state & ButtonFlags_Begin) m_mouseButton[i].state ^= ButtonFlags_Begin;
        /*else*/
        if (m_mouseButton[i].state & ButtonFlags_End) m_mouseButton[i].state = ButtonFlags_Off;
    }

    // Flush key buffer
    m_nbKeys = 0;
}

Rect UIContext::placeRect(const Rect & r) {
    Group& group = m_groupStack[m_groupIndex];
    int layout = group.flags & GroupFlags_LayoutMask;
    int alignment = group.flags & GroupFlags_AlignMask;

    Rect rect = r;
    if (layout == GroupFlags_LayoutNone) {
        // Translate rect to absolute coordinates.
        rect.x += group.bounds.x;
        rect.y += group.bounds.y;
    } else if (layout == GroupFlags_LayoutVertical) {
        // Vertical behavior
        if (alignment & GroupFlags_AlignTop) {
            // Move down bounds.y with the space for the new rect
            group.bounds.y -= ((group.bounds.h > 0) * group.space + rect.h);

            // Widget's y is the group bounds.y
            rect.y = group.bounds.y;
        } else {
            rect.y = group.bounds.y + group.bounds.h + (group.bounds.h > 0) * group.space;
        }
        // Add space after first object inserted in the group
        group.bounds.h += (group.bounds.h > 0) * group.space + rect.h;

        // Horizontal behavior
        if (alignment & GroupFlags_AlignRight) {
            rect.x += group.bounds.x + group.bounds.w - rect.w;

            int minBoundX = std::min(group.bounds.x, rect.x);
            group.bounds.w = group.bounds.x + group.bounds.w - minBoundX;
            group.bounds.x = minBoundX;
        } else {
            group.bounds.w = std::max(group.bounds.w, rect.x + rect.w);
            rect.x += group.bounds.x;
        }
    } else if (layout == GroupFlags_LayoutHorizontal) {
        // Horizontal behavior
        if (alignment & GroupFlags_AlignRight) {
            // Move left bounds.x with the space for the new rect
            group.bounds.x -= ((group.bounds.w > 0) * group.space + rect.w);

            // Widget's x is the group bounds.x
            rect.x = group.bounds.x;
        } else {
            rect.x = group.bounds.x + group.bounds.w + (group.bounds.w > 0) * group.space;
        }
        // Add space after first object inserted in the group
        group.bounds.w += (group.bounds.w > 0) * group.space + rect.w;

        // Vertical behavior
        if (alignment & GroupFlags_AlignTop) {
            rect.y += group.bounds.y + group.bounds.h - rect.h;

            int minBoundY = std::min(group.bounds.y, rect.y);
            group.bounds.h = group.bounds.y + group.bounds.h - minBoundY;
            group.bounds.y = minBoundY;
        } else {
            group.bounds.h = std::max(group.bounds.h, rect.y + rect.h);
            rect.y += group.bounds.y;
        }
    }
    
    return rect;
}

void UIContext::beginGroup(int flags, const Rect& r) {
    // Push one more group.
    Group & parentGroup = m_groupStack[m_groupIndex];
    m_groupIndex++;
    Group & newGroup = m_groupStack[m_groupIndex];

    // Assign layout behavior
    int parentLayout = parentGroup.flags & GroupFlags_LayoutMask;
    int parentAlign = parentGroup.flags & GroupFlags_AlignMask;

    // If the flags ask to force the layout then keep the newcanvas layout as is
    // otherwise, adapt it to the parent's behavior
    if ( ! (flags & GroupFlags_LayoutForce) || ! (flags & GroupFlags_LayoutNone) ) {
        // If default then use parent style except if none layout => default fallback
        if ( flags & GroupFlags_LayoutDefault ) {
            if ( parentLayout &  GroupFlags_LayoutNone )
                flags = GroupFlags_LayoutDefaultFallback;
            else
                flags = parentGroup.flags;
        } else if (   parentLayout & ( GroupFlags_LayoutVertical | GroupFlags_LayoutHorizontal)
                && flags & ( GroupFlags_LayoutVertical | GroupFlags_LayoutHorizontal) ) {
            flags = (flags & GroupFlags_AlignXMask) | parentAlign;
        }
    }

    newGroup.margin = ((flags & GroupFlags_LayoutNoMargin) == 0) * m_painter->getCanvasMargin();
    newGroup.space = ((flags & GroupFlags_LayoutNoSpace) == 0) * m_painter->getCanvasSpace();
    newGroup.flags = flags;
    //int newLayout = flags & GroupFlags_LayoutMask;
    int newAlign = flags & GroupFlags_AlignMask;
    int newStart = flags & GroupFlags_StartMask;

    // Place a regular rect in current group, this will be the new group rect start pos
    Rect rect = r;

    // Don't modify parent group bounds yet, done in endGroup()
    // Right now place the new group rect
    if ( parentLayout == GroupFlags_LayoutNone) {
        // Horizontal behavior.
        rect.x +=   parentGroup.bounds.x + newGroup.margin
                + ((newStart & GroupFlags_StartRight) > 0) * parentGroup.bounds.w
                - ((newAlign & GroupFlags_AlignRight) > 0) * (2 * newGroup.margin + rect.w);

        // Vertical behavior.
        rect.y +=   parentGroup.bounds.y + newGroup.margin
                + ((newStart & GroupFlags_StartTop) > 0) * parentGroup.bounds.h
                - ((newAlign & GroupFlags_AlignTop) > 0) * (2 * newGroup.margin + rect.h);
    } else if ( parentLayout == GroupFlags_LayoutVertical) {
        // Horizontal behavior.
        rect.x +=   parentGroup.bounds.x + newGroup.margin
                + ((parentAlign & GroupFlags_AlignRight) > 0) * (parentGroup.bounds.w - 2 * newGroup.margin - rect.w);

        // Vertical behavior.
        if (parentAlign & GroupFlags_AlignTop) {
            rect.y += parentGroup.bounds.y - ((parentGroup.bounds.h > 0) * parentGroup.space) - newGroup.margin - rect.h;
        } else {
            rect.y += parentGroup.bounds.y + parentGroup.bounds.h + (parentGroup.bounds.h > 0) * parentGroup.space + newGroup.margin;
        }
    } else if ( parentLayout == GroupFlags_LayoutHorizontal) {
        // Horizontal behavior.
        if (parentAlign & GroupFlags_AlignRight) {
            rect.x += parentGroup.bounds.x - ((parentGroup.bounds.w > 0) * parentGroup.space) - newGroup.margin - rect.w;
        } else {
            rect.x += parentGroup.bounds.x + parentGroup.bounds.w + (parentGroup.bounds.w > 0) * parentGroup.space + newGroup.margin;
        }

        // Vertical behavior.
        rect.y +=   parentGroup.bounds.y + newGroup.margin
                + ((parentAlign & GroupFlags_AlignTop) > 0) * (parentGroup.bounds.h - 2 * newGroup.margin - rect.h);
    }

    newGroup.bounds = rect;
}

void UIContext::endGroup() {
    // Pop the new group.
    Group & newGroup = m_groupStack[m_groupIndex];
    m_groupIndex--;
    Group & parentGroup = m_groupStack[m_groupIndex];

    // add any increment from the embedded group
    if (parentGroup.flags & ( GroupFlags_LayoutVertical | GroupFlags_LayoutHorizontal ) ) {
        int maxBoundX = std::max(parentGroup.bounds.x + parentGroup.bounds.w, newGroup.bounds.x + newGroup.bounds.w + newGroup.margin);
        int minBoundX = std::min(parentGroup.bounds.x, newGroup.bounds.x - newGroup.margin);
        parentGroup.bounds.x = minBoundX;
        parentGroup.bounds.w = maxBoundX - minBoundX;

        int maxBoundY = std::max(parentGroup.bounds.y + parentGroup.bounds.h, newGroup.bounds.y + newGroup.bounds.h + newGroup.margin);
        int minBoundY = std::min(parentGroup.bounds.y, newGroup.bounds.y - newGroup.margin);
        parentGroup.bounds.y = minBoundY;
        parentGroup.bounds.h = maxBoundY - minBoundY;
    }
}

void UIContext::beginFrame(int flags, const Rect& rect, int /*style*/) {
    beginGroup(flags, rect);
}

void UIContext::endFrame() {
    endGroup();
    m_painter->drawFrame( m_groupStack[m_groupIndex + 1].bounds, m_groupStack[m_groupIndex + 1].margin, 0);
}

bool UIContext::beginPanel(Rect & r, const char * text, bool * isUnfold, int flags, int style) {
    Rect rt, ra;
    Rect rpanel = m_painter->getPanelRect(Rect(r.x, r.y), m_font->getTextRect(text), rt, ra);

    if (flags & GroupFlags_LayoutDefault)
        flags = GroupFlags_LayoutDefaultFallback;
    beginGroup( ( flags | GroupFlags_LayoutNoMargin | GroupFlags_LayoutNoSpace ) & GroupFlags_StartXMask , rpanel );

    Rect rect = m_groupStack[m_groupIndex].bounds;

    bool focus = hasFocus(rect);
    bool hover = isHover(rect);

    if (focus) {
        m_uiOnFocus = true;

        r.x += m_currentCursor.x - m_mouseButton[0].cursor.x;
        r.y += m_currentCursor.y - m_mouseButton[0].cursor.y;
        rect.x += m_currentCursor.x - m_mouseButton[0].cursor.x;
        rect.y += m_currentCursor.y - m_mouseButton[0].cursor.y;

        m_mouseButton[0].cursor    = m_currentCursor;
    }

    if (m_mouseButton[0].state & ButtonFlags_End && focus && overlap( Rect(rect.x + ra.x, rect.y + ra.y, ra.w, ra.h) , m_currentCursor)) {
        if ( isUnfold )
            *isUnfold = !*isUnfold;
    }

    m_painter->drawPanel(rect, text, rt, ra, *isUnfold, hover, focus, style);

    if (isUnfold && *isUnfold) {
        beginFrame( flags, Rect(0, 0, r.w, r.h) );
        return true;
    } else {
        endGroup();
        return false;
    }
}

void UIContext::endPanel() {
    endFrame();
    endGroup();
}

void UIContext::doLabel(const Rect & r, const char * text, int style) {
    Rect rt;
    Rect rect = placeRect(m_painter->getLabelRect(r, m_font->getTextRect(text), rt));
    //~ printf("label place %d,%d %d,%d\n", rect.x, rect.y, rect.w, rect.h);
    m_painter->drawLabel(rect, text, rt, isHover(rect), style);
}

void UIContext::doGraph( const Rect& r, const int *values, const int n )
{
    Rect rg;
    Rect rect= placeRect(m_painter->getGraphRect(r, values, n, rg));
    m_painter->drawGraph(rect, values, n, rg);
}

void UIContext::doTimebar( const Rect& r, const char *text, const float start, const float stop )
{
    Rect rt;
    Rect rb;
    Rect rect= placeRect(m_painter->getTimebarRect(r, m_font->getTextRect(text), rt, start, stop, rb));
    m_painter->drawTimebar(rect, text, rt, start, stop, rb);
}


bool UIContext::doMatrix( const Rect& r, const RGB8 *colors, const int numColors, int *selected, int *hovered )
{
    Rect rc;
    Rect rect= placeRect(m_painter->getMatrixRect(r, numColors, rc));
    
    int cell= -1;
    bool focus = hasFocus(rect);
    if(focus) {
        //~ int rows= rect.h / rc.h;
        int cols= rect.w / rc.w;
        //~ printf("place %d,%d %d,%d, cell %d,%d %d,%d\n", rect.x, rect.y, rect.w, rect.h, rc.x, rc.y, rc.w, rc.h);
        //~ printf("matrix %d %dx%d\n", numColors, rows, cols);
        cell= (rect.h - m_currentCursor.y + rect.y) / rc.h * cols + (m_currentCursor.x - rect.x) / rc.w;
        //~ printf("cell %d\n", cell);
    }
    
    m_painter->drawMatrix(rect, colors, numColors, selected, &cell);
    
    if(hovered) *hovered= cell;
    
    if(focus) m_uiOnFocus = true; 
    if(m_mouseButton[0].state & ButtonFlags_End && cell != -1) {
        if(selected) {
            if(*selected != cell) *selected= cell;
            else *selected= -1;
        }
        return true;
    }
    
    return false;
}


bool UIContext::doButton(const Rect & r, const char * text, bool * state, int style) {
    Rect rt;
    Rect rect = placeRect(m_painter->getButtonRect(r, m_font->getTextRect(text), rt));

    bool focus = hasFocus(rect);
    bool hover = isHover(rect);
    bool isDown = false;

    if ( state ) {
        isDown = *state;
    } else {
        isDown = (m_mouseButton[0].state & ButtonFlags_On) && hover && focus;
    }

    m_painter->drawButton(rect, text, rt, isDown, hover, focus, style);

    if (focus) {
        m_uiOnFocus = true;
    }

    if (m_mouseButton[0].state & ButtonFlags_End && focus && overlap(rect, m_currentCursor)) {
        if ( state )
            *state = !*state;
        return true;
    }

    return false;
}

bool UIContext::doCheckButton(const Rect & r, const char *text, bool *state, int style) {
    Rect rt, rc;
    Rect rect = placeRect(m_painter->getCheckRect(r, m_font->getTextRect(text), rt, rc));

    bool focus = hasFocus(rect);
    bool hover = isHover(rect);

    m_painter->drawCheckButton(rect, text, rt, rc, (state) && (*state), hover, focus, style);

    if (focus) {
        m_uiOnFocus = true;
    }

    if (m_mouseButton[0].state & ButtonFlags_End && focus && overlap(rect, m_currentCursor)) {
        if ( state )
            *state = !*state;
        return true;
    }

    return false;
}

bool UIContext::doRadioButton(int reference, const Rect& r, const char *text, int *selected, int style) {
    Rect rr, rt;
    Rect rect = placeRect(m_painter->getRadioRect(r, m_font->getTextRect(text), rt, rr));

    bool focus = hasFocus(rect);
    bool hover = isHover(rect);

    m_painter->drawRadioButton(rect, text, rt, rr, (selected) && (reference == *selected), hover, focus, style);

    if (focus) {
        m_uiOnFocus = true;
    }

    if (m_mouseButton[0].state & ButtonFlags_End && focus && overlap(rect, m_currentCursor)) {
        if (selected)
            *selected = reference;
        return true;
    }

    return false;
}

bool UIContext::doListItem(int index, const Rect & r, const char *text, int *selected, int style) {
    Rect rt;
    Rect rect = placeRect(m_painter->getItemRect(r, m_font->getTextRect(text), rt));

    m_painter->drawListItem(rect, text, rt, (selected) && (index == *selected), isHover(rect), style);

    return isHover(rect);
}

bool UIContext::doListBox(const Rect & r, int numOptions, const char *options[], int *selected, int style) {
    Rect ri;
    Rect rt;
    Rect rect = placeRect(m_painter->getListRect(r, numOptions, m_font->getTextRect(numOptions, options), ri, rt));

    bool focus = hasFocus(rect);
    bool hover = isHover(rect);
    int hovered = -1;

    if ( hover ) {
        hovered = numOptions - 1 - (m_currentCursor.y - (rect.y + ri.y)) / (ri.h);
    }

    int lSelected = -1;
    if (selected)
        lSelected = *selected;
    m_painter->drawListBox(rect, numOptions, options, ri, rt, lSelected, hovered, style);

    if (focus) {
        m_uiOnFocus = true;
    }

    if (m_mouseButton[0].state & ButtonFlags_End && focus && overlap(rect, m_currentCursor) && (lSelected != hovered) ) {
        if (selected)
            *selected = hovered;
        return true;
    }

    return false;
}

bool UIContext::doComboBox(const Rect & r, int numOptions, const char *options[], int *selected, int style) {
    // First get the rect of the COmbo box itself and do some test with it
    Rect rt, ra;
    Rect rect = placeRect(m_painter->getComboRect(r, numOptions, m_font->getTextRect(numOptions, options), *selected, rt, ra));

    bool focus = hasFocus(rect);
    bool hover = isHover(rect);

    if (focus) {
        m_uiOnFocus = true;

        // then if the combo box has focus, we can look for the geometry of the options frame
        Rect ro, ri, rit;
        ro = m_painter->getComboOptionsRect(rect, numOptions, m_font->getTextRect(numOptions, options), ri, rit);

        int hovered = -1;
        bool hoverOptions = overlap(ro, m_currentCursor);
        if ( hoverOptions ) {
            hovered = numOptions - 1 - (m_currentCursor.y - (ro.y + ri.y)) / (ri.h);
        }

        // draw combo anyway
        m_painter->drawComboBox( rect, numOptions, options, rt, ra, *selected, hover, focus, style);

        // draw options
        m_painter->drawComboOptions( ro, numOptions, options, ri, rit, *selected, hovered, hover, focus, style);


        // When the widget get the focus, cache the focus point
        if (!m_twoStepFocus) {
            if (hover && m_mouseButton[0].state & ButtonFlags_End) {
                m_focusPoint = m_mouseButton[0].cursor;
                m_twoStepFocus = true;
            }
        } else {
            // Else Release the 2level focus when left mouse down out or up anyway
            // replace the stored left mouse down pos with the focus point to keep focus
            // on this widget for the next widget rendered in the frame
            if (!(hoverOptions || hover)
            && (m_mouseButton[0].state & ButtonFlags_Begin || m_mouseButton[0].state & ButtonFlags_End)) {
                m_twoStepFocus = false;
            } else if ( (hoverOptions || hover) && m_mouseButton[0].state & ButtonFlags_End) {
                m_mouseButton[0].cursor = m_focusPoint;
                m_twoStepFocus = false;
            }

            if (hoverOptions && m_mouseButton[0].state & ButtonFlags_Begin) {
                m_mouseButton[0].cursor = m_focusPoint;
            }
        }

        // On mouse left bouton up, then select it
        if ( (hovered > -1) && (hovered < numOptions) && (m_mouseButton[0].state & ButtonFlags_End ) ) {
            *selected = hovered;
            return true;
        }
    } else {
        m_painter->drawComboBox( rect, numOptions, options, rt, ra, *selected, hover, focus, style);
    }

    return false;
}

bool UIContext::doHorizontalSlider(const Rect & r, float min, float max, float *value, int style) {
    // Map current value to 0-1.
    float f = (*value - min) / (max - min);
    if (f < 0.0f) f = 0.0f;
    else if (f > 1.0f) f = 1.0f;

    Rect rs;
    Rect rc;
    Rect rect = placeRect(m_painter->getHorizontalSliderRect( r, rs, f, rc ));

    bool changed = false;
    if (hasFocus(rect)) {
        m_uiOnFocus = true;

        int xs = rect.x + rs.x + rc.w / 2;
        int x = m_currentCursor.x - xs;

        if (x < 0) x = 0;
        else if (x > rs.w) x = rs.w;

        rc.x = x;

        float f = (float(x)) / ((float) rs.w);
        f = f * (max - min) + min;

        if (fabs(f - *value) > (max - min) * 0.01f) {
            changed = true;
            *value = f;
        }
    }

    m_painter->drawHorizontalSlider(rect, rs, f, rc, isHover(rect), style);

    return changed;
}

bool UIContext::doLineEdit(const Rect & r, char *text, int maxTextLength, int *nbCharsReturned, int style) {
    Rect rt;
    Rect rect = placeRect(m_painter->getLineEditRect(r, m_font->getTextRect(text), rt));

    bool focus = hasFocus(rect);
    bool hover = isHover(rect);
    bool result = false;
    int carretPos = -1;

    if (focus) {
        m_uiOnFocus = true;

        // When the widget get the focus, cache the focus point
        if (!m_twoStepFocus) {
            m_twoStepFocus = true;
            m_focusPoint = Point(rect.x, rect.y);
        } else {
            // Else Release the 2level focus when left mouse down out or up anyway
            // replace the stored left mouse down pos with the focus point to keep focus
            // on this widget for the next widget rendered in the frame
            if (!hover
            && (m_mouseButton[0].state & ButtonFlags_Begin || m_mouseButton[0].state & ButtonFlags_End)) {
                m_twoStepFocus = false;
                m_focusCaretPos = -1;
            }

            if (hover && m_mouseButton[0].state & ButtonFlags_Begin) {
                m_mouseButton[0].cursor = m_focusPoint;
            }
        }
        
        // if drop file is buffered, replace text
        if(hover && m_fileBuffer && m_fileBuffer[0] != 0) {
            m_focusCaretPos= 0;
            strncpy(text, m_fileBuffer, maxTextLength -1);
            text[maxTextLength -1]= 0;
            m_fileBuffer[0]= 0;
            result= true;
        }
        
        // Eval caret pos on every click hover
        if ( hover && m_mouseButton[0].state & ButtonFlags_Begin )
            m_focusCaretPos = m_font->getPickedCharNb(text, Point( m_currentCursor.x - rt.x - rect.x, m_currentCursor.y - rt.y - rect.y));
        
        // If keys are buffered, apply input to the edited text
        if ( m_nbKeys ) {
            int textLength = (int) strlen(text);

            if (m_focusCaretPos == -1)
                m_focusCaretPos = textLength;

            int nbKeys = m_nbKeys;
            int keyNb = 0;
            while (nbKeys) {
                // filter for special keys
                // Enter, quit edition
                if ( m_keyBuffer[keyNb] == Key_Enter ) {
                    nbKeys = 1;
                    m_twoStepFocus = false;
                    m_focusCaretPos = -1;
                }
                else if( m_keyBuffer[keyNb] > Key_Enter)
                {
                    // Special keys
                    if( m_keyBuffer[keyNb] == Key_Left ) {
                        // move cursor left one char
                        if (m_focusCaretPos > 0)
                            m_focusCaretPos--;
                    }
                    else if( m_keyBuffer[keyNb] == Key_Right ) {
                        // move cursor right one char
                        if (m_focusCaretPos < textLength)
                            m_focusCaretPos++;
                    }
                    else if( m_keyBuffer[keyNb] == Key_Home ) {
                        m_focusCaretPos = 0;
                    }
                    else if( m_keyBuffer[keyNb] == Key_End ) {
                        m_focusCaretPos = textLength;
                    }
                    // Delete, move the chars >= carret back 1, carret stay in place
                    else if ( m_keyBuffer[keyNb] == Key_Delete ) {
                        if (m_focusCaretPos < textLength) {
                            memmove( text + m_focusCaretPos, text + m_focusCaretPos + 1, textLength - m_focusCaretPos);
                            textLength--;
                        }
                    }
                    // Backspace, move the chars > carret back 1, carret move back 1
                    else if ( m_keyBuffer[keyNb] == Key_Backspace) {
                        if (m_focusCaretPos > 0) {
                            if (m_focusCaretPos < textLength) {
                                memmove( text + m_focusCaretPos - 1, text + m_focusCaretPos, textLength - m_focusCaretPos);
                            }
                            m_focusCaretPos--;
                            textLength--;
                        }
                    }
                    else if ( m_keyBuffer[keyNb] == Key_Tab ) {
                        // nothing
                    }
                }
                // Regular char, append it to the edited string
                else if ( textLength < maxTextLength) {
                    if (m_focusCaretPos < textLength) {
                        memmove( text + m_focusCaretPos + 1, text + m_focusCaretPos, textLength - m_focusCaretPos);
                    }
                    text[m_focusCaretPos] = m_keyBuffer[keyNb];
                    m_focusCaretPos++;
                    textLength++;
                }
                
                keyNb++;
                nbKeys--;
            }
            
            text[textLength]= '\0';
            if(nbCharsReturned) *nbCharsReturned= textLength;
            result= true;
        }
        
        carretPos= m_focusCaretPos;
    }
    
    m_painter->drawLineEdit(rect, text, rt, m_font->getTextLineWidthAt(text, carretPos), focus, hover, style);
    
    return result;
}

void UIContext::setCursor(int x, int y) {
    m_currentCursor.x = x;
    m_currentCursor.y = m_window.h - y;
}

bool UIContext::overlap(const Rect & rect, const Point & p) {
    return p.x >= rect.x && p.x < rect.x + rect.w
        && p.y >= rect.y && p.y < rect.y + rect.h;
}

// Left mouse button down, and down cursor inside rect.
bool UIContext::hasFocus(const Rect & rect) {
    if (m_twoStepFocus) {
        return overlap(rect, m_focusPoint);
    } else {
        return (m_mouseButton[0].state & ButtonFlags_On) && overlap(rect, m_mouseButton[0].cursor);
    }
}

bool UIContext::isHover(const Rect & rect) {
    if (m_uiOnFocus && !hasFocus(rect)) 
        return false;
    return overlap(rect, m_currentCursor);
}

}       // namespace
