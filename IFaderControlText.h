//
//  IFaderControlText.h
//  ReplicantDelay
//
//  Class for displaying text along with a knob.
//
//  Created by Brad Guesman on 1/13/18.
//

#ifndef IFaderControlText_h
#define IFaderControlText_h
#include "IPlug_include_in_plug_hdr.h"
#include "IControl.h"
#include "resource.h"


class IFaderControlText : public IFaderControl
{
private:
    IRECT mTextRECT, mImgRECT;
    IBitmap mBitmap;
    
public:
    IFaderControlText(IPlugBase* pPlug, IRECT pR, int sliderLen, int paramIdx, IBitmap* pBitmap, EDirection direction, bool onlyHandle, IText* pText, int textHorizOffset, int textVertOffset)
    : IFaderControl(pPlug, pR.L, pR.T, sliderLen, paramIdx, pBitmap, direction, onlyHandle), mBitmap(*pBitmap)
    {
        mText = *pText;
        mRECT.B = pR.B; // so that the GUI refreshes the text component
        mRECT.R = pR.R;
        mTextRECT = IRECT(mRECT.L - textHorizOffset, mRECT.B - textVertOffset, mRECT.R, mRECT.B);
        mImgRECT = IRECT(mRECT.L, mRECT.T, &mBitmap);
        mDisablePrompt = true;
    }
    
    ~IFaderControlText() {}
    
    bool Draw(IGraphics* pGraphics)
    {
//        pGraphics->FillIRect(&COLOR_WHITE, &mTextRECT);
//        IFaderControl::Draw(pGraphics);
        IRECT r = GetHandleRECT();
        pGraphics->DrawBitmap(&mBitmap, &r, 1, &mBlend);
        
        char disp[20];
        mPlug->GetParam(mParamIdx)->GetDisplayForHost(disp);
        
        if (CSTR_NOT_EMPTY(disp))
        {
            return pGraphics->DrawIText(&mText, disp, &mTextRECT);
        }
        return true;
    }
    
    void OnMouseDown(int x, int y, IMouseMod* pMod)
    {
        if (mTextRECT.Contains(x, y)) PromptUserInput(&mTextRECT);
#ifdef RTAS_API
        else if (pMod->A)
        {
            if (mDefaultValue >= 0.0)
            {
                mValue = mDefaultValue;
                SetDirty();
            }
        }
#endif
        else
        {
            OnMouseDrag(x, y, 0, 0, pMod);
        }
    }
    
    void OnMouseDblClick(int x, int y, IMouseMod* pMod)
    {
#ifdef RTAS_API
        PromptUserInput(&mTextRECT);
#else
        if (mDefaultValue >= 0.0)
        {
            mValue = mDefaultValue;
            SetDirty();
        }
#endif
    }
    
};

#endif /* IFaderControlText_h */

