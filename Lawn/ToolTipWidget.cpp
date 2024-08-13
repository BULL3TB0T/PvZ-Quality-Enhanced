#include "../Resources.h"
#include "ToolTipWidget.h"
#include "../GameConstants.h"
#include "../Sexy.TodLib/TodCommon.h"
#include "../SexyAppFramework/Font.h"
#include "../Sexy.TodLib/TodStringFile.h"

using namespace Sexy;

ToolTipWidget::ToolTipWidget()
{
	mX = 0;
	mY = 0;
	mWidth = 0;
	mHeight = 0;
	mVisible = 1;
	mCenter = 0;
	mMinLeft = 0;
	mMaxBottom = BOARD_HEIGHT;
	mGetsLinesWidth = 0;
	mWarningFlashCounter = 0;
}

void ToolTipWidget::GetLines(std::vector<SexyString>& theLines)
{
	int aLineWidth = 0;
	unsigned int aIndexStart = 0;
	unsigned int aIndexInLine = 0;

	Font* aFont = OLD_STYLE_TOOLTIP ? FONT_PICO129 : FONT_BRIANNETOD12;
	SexyString aLabel = TodStringTranslate(mLabel);
	while (aIndexInLine != aLabel.size())
	{
		while (aIndexInLine < aLabel.size() && aLabel[aIndexInLine] != ' ' && aLabel[aIndexInLine] != '\n')
		{
			aLineWidth += aFont->CharWidth(aLabel[aIndexInLine]);
			aIndexInLine++;
		}

		if (aIndexInLine != aLabel.size() && aLineWidth < mGetsLinesWidth && aLabel[aIndexInLine] != '\n')
		{
			aLineWidth += aFont->CharWidth(aLabel[aIndexInLine]);
			aIndexInLine++;
		}
		else
		{
			SexyString aLine = aLabel.substr(aIndexStart, aIndexInLine - aIndexStart);
			aLineWidth = 0;
			theLines.push_back(aLine);

			if (aIndexInLine < aLabel.size() && aLabel[aIndexInLine] == '\n')
			{
				aIndexInLine++;
			}
			while (aIndexInLine < aLabel.size() && aLabel[aIndexInLine] == ' ')
			{
				aIndexInLine++;
			}

			aIndexStart = aIndexInLine;
		}
	}
}

void ToolTipWidget::CalculateSize()
{
	std::vector<SexyString> aLines;

	SexyString aTitle = TodStringTranslate(mTitle);
	SexyString aWarningText = TodStringTranslate(mWarningText);
	Font* aTitleFont = OLD_STYLE_TOOLTIP ? FONT_TINYBOLD : FONT_BRIANNETOD16;
	Font* aFont = OLD_STYLE_TOOLTIP ? FONT_PICO129 : FONT_BRIANNETOD12;
	int aTitleWidth = aTitleFont->StringWidth(aTitle);
	int aWarningWidth = aFont->StringWidth(aWarningText);
	int aMaxWidth = max(aTitleWidth, aWarningWidth);

	mGetsLinesWidth = max(aMaxWidth - 30, 100);
	GetLines(aLines);

	for (int i = 0; i < aLines.size(); i++)
	{
		int aLineWidth = aFont->StringWidth(aLines[i]);
		aMaxWidth = max(aMaxWidth, aLineWidth);
	}

	int aHeight = 6;
	if (!aTitle.empty())
	{
		aHeight = aTitleFont->GetAscent() + 8;
	}
	if (!aWarningText.empty())
	{
		aHeight += aFont->GetAscent() + 2;
	}
	aHeight += aLines.size() * aFont->GetAscent();

	mWidth = aMaxWidth + 10;
	mHeight = aHeight + aLines.size() * 2 - 2;
}

void ToolTipWidget::SetLabel(const SexyString& theLabel)
{
	mLabel = theLabel;
	CalculateSize();
}

void ToolTipWidget::SetTitle(const SexyString& theTitle)
{
	mTitle = theTitle;
	CalculateSize();
}

void ToolTipWidget::SetWarningText(const SexyString& theWarningText)
{
	mWarningText = theWarningText;
	CalculateSize();
}

void ToolTipWidget::Draw(Graphics* g)
{
	if (!mVisible)
		return;

	int aPosX = mX;
	if (mCenter)
	{
		aPosX -= mWidth / 2;
	}
	if (mMinLeft - g->mTransX > aPosX)  // aPosX + g->mTransX < mMinLeft
	{
		aPosX = mMinLeft - (int)g->mTransX;
	}
	else if (aPosX + mWidth + g->mTransX > BOARD_WIDTH)
	{
		aPosX = BOARD_WIDTH - g->mTransX - mWidth;
	}

	int aPosY = mY;
	if (-g->mTransY > aPosY)  // aPosY + g->mTransY > 0
	{
		aPosY = (int)-g->mTransY;
	}
	else if (mMaxBottom < mY + mHeight + g->mTransY)
	{
		aPosY = mMaxBottom - (int)g->mTransY - mHeight;
	}

	g->SetColor(Color(255, 255, 200, 255));
	g->FillRect(aPosX, aPosY, mWidth, mHeight);
	g->SetColor(Color::Black);
	g->DrawRect(aPosX, aPosY, mWidth - 1, mHeight - 1);
	aPosY++;

	Font* aTitleFont = OLD_STYLE_TOOLTIP ? FONT_TINYBOLD : FONT_BRIANNETOD16;
	SexyString aTitle = TodStringTranslate(mTitle);
	if (!aTitle.empty())
	{
		g->SetFont(aTitleFont);
		g->DrawString(aTitle, aPosX + (mWidth - aTitleFont->StringWidth(aTitle)) / 2, aPosY + aTitleFont->GetAscent());
		aPosY += aTitleFont->GetAscent() + 2;
	}

	Font* aWarningFont = OLD_STYLE_TOOLTIP ? FONT_PICO129 : FONT_BRIANNETOD12;
	SexyString aWarningText = TodStringTranslate(mWarningText);
	if (!aWarningText.empty())
	{
		g->SetFont(aWarningFont);
		int x = aPosX + (mWidth - aWarningFont->StringWidth(aWarningText)) / 2;
		int y = aPosY + aWarningFont->GetAscent();

		Color aWarningColor(255, 0, 0);
		if (mWarningFlashCounter > 0 && mWarningFlashCounter % 20 < 10)
		{
			aWarningColor = Color(0, 0, 0);
		}

		g->SetColor(aWarningColor);
		g->DrawString(aWarningText, x, y);
		g->SetColor(Color::Black);

		aPosY += aWarningFont->GetAscent() + 2;
	}

	std::vector<SexyString> aLines;
	GetLines(aLines);

	Font* aFont = OLD_STYLE_TOOLTIP ? FONT_PICO129 : FONT_BRIANNETOD12;
	g->SetFont(aFont);
	for (int i = 0; i < aLines.size(); i++)
	{
		SexyString aLine = aLines[i];
		g->DrawString(aLine, aPosX + (mWidth - aFont->StringWidth(aLine)) / 2, aPosY + aFont->GetAscent());
		aPosY += aFont->GetAscent() + 2;
	}
}
