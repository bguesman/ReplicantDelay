//
//  IKnobMultiControlText.h
//  ReplicantDelay
//
//  Class for displaying text along with a knob.
//
//  Created by Brad Guesman on 1/13/18.
//

#ifndef IKnobMultiControlText_h
#define IKnobMultiControlText_h
#include "IPlug_include_in_plug_hdr.h"
#include "IControl.h"
#include "resource.h"


class IKnobMultiControlText : public IKnobControl
{
private:
    IRECT mTextRECT, mImgRECT;
    IBitmap mBitmap;
    
public:
    IKnobMultiControlText(IPlugBase* pPlug, IRECT pR, int paramIdx, IBitmap* pBitmap, IText* pText)
    : IKnobControl(pPlug, pR, paramIdx), mBitmap(*pBitmap)
    {
        mText = *pText;
        mTextRECT = IRECT(mRECT.L, mRECT.B - 20, mRECT.R, mRECT.B);
        mImgRECT = IRECT(mRECT.L, mRECT.T, &mBitmap);
        mDisablePrompt = false;
    }
    
    ~IKnobMultiControlText() {}
    
    bool Draw(IGraphics* pGraphics)
    {
        int i = 1 + int(0.5 + mValue * (double) (mBitmap.N - 1));
        i = BOUNDED(i, 1, mBitmap.N);
        pGraphics->DrawBitmap(&mBitmap, &mImgRECT, i, &mBlend);
//        pGraphics->FillIRect(&COLOR_WHITE, &mTextRECT);
        
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

#endif /* IKnobMultiControlText_h */
