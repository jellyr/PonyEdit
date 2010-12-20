#include "syntax/syntaxrule.h"
#include "main/tools.h"

QMap<QString, SyntaxRule::Type> SyntaxRule::sTypeMap;
bool SyntaxRule::sTypeMapInitialized = false;

SyntaxRule::SyntaxRule(SyntaxRule* parent, const QString& name, const QXmlAttributes& attributes)
{
	if (!sTypeMapInitialized)
	{
		sTypeMap.insert("detectchar", DetectChar);
		sTypeMap.insert("detect2chars", Detect2Chars);
		sTypeMap.insert("anychar", AnyChar);
		sTypeMap.insert("stringdetect", StringDetect);
		sTypeMap.insert("worddetect", WordDetect);
		sTypeMap.insert("regexpr", RegExpr);
		sTypeMap.insert("keyword", Keyword);
		sTypeMap.insert("int", Int);
		sTypeMap.insert("float", Float);
		sTypeMap.insert("hlcoct", HlCOct);
		sTypeMap.insert("hlchex", HlCHex);
		sTypeMap.insert("hlcstringchar", HlCStringChar);
		sTypeMap.insert("hlcchar", HlCChar);
		sTypeMap.insert("rangedetect", RangeDetect);
		sTypeMap.insert("linecontinue", LineContinue);
		sTypeMap.insert("detectspaces", DetectSpaces);
		sTypeMap.insert("detectidentifier", DetectIdentifier);
		sTypeMap.insert("includerules", IncludeRules);
		sTypeMapInitialized = true;
	}

	mName = name;
	mParent = parent;
	mValid = false;

	QString lcName = name.toLower();
	if (sTypeMap.contains(lcName))
	{
		mType  = sTypeMap.value(lcName);

		if (mType == IncludeRules && mParent != NULL)
		{
			qDebug() << "Warning: Include inside parent rule; disregarding!";
			return;
		}

		mAttribute = Tools::getStringXmlAttribute(attributes, "attribute");
		mContext = Tools::getStringXmlAttribute(attributes, "context");
		mBeginRegion = Tools::getStringXmlAttribute(attributes, "beginregion");
		mEndRegion = Tools::getStringXmlAttribute(attributes, "endregion");
		mLookAhead = Tools::getIntXmlAttribute(attributes, "lookahead", 0);
		mFirstNonSpace = Tools::getIntXmlAttribute(attributes, "firstnonspace", 0);
		mColumn = Tools::getIntXmlAttribute(attributes, "column", 0);
		mCharacterA = Tools::getCharXmlAttribute(attributes, "char");
		mCharacterB = Tools::getCharXmlAttribute(attributes, "char1");
		mString = Tools::getStringXmlAttribute(attributes, "string");
		mCaseInsensitive = Tools::getIntXmlAttribute(attributes, "insensitive", 0);
		mDynamic = Tools::getIntXmlAttribute(attributes, "dynamic", 0);
		mMinimal = Tools::getIntXmlAttribute(attributes, "minimal", 0);
		mIncludeAttrib = Tools::getIntXmlAttribute(attributes, "includeAttrib", 0);

		mValid = true;
	}
	else
	{
		qDebug() << "Unrecognized rule type: " << name;
	}
}

SyntaxRule::SyntaxRule(SyntaxRule* parent, const QSharedPointer<SyntaxRule>& other)
{
	mParent = parent;
	mName = other->mName;
	mType = other->mType;
	mValid = other->mValid;

	mAttribute = other->mAttribute;
	mContext = other->mContext;
	mBeginRegion = other->mBeginRegion;
	mEndRegion = other->mEndRegion;
	mLookAhead = other->mLookAhead;
	mFirstNonSpace = other->mFirstNonSpace;
	mColumn = other->mColumn;

	mCharacterA = other->mCharacterA;
	mCharacterB = other->mCharacterB;
	mString = other->mString;
	mCaseInsensitive = other->mCaseInsensitive;
	mDynamic = other->mDynamic;
	mMinimal = other->mMinimal;
	mIncludeAttrib = other->mIncludeAttrib;

	foreach (const QSharedPointer<SyntaxRule>& otherChild, other->mChildRules)
		mChildRules.append(QSharedPointer<SyntaxRule>(new SyntaxRule(this, otherChild)));
}

SyntaxRule::~SyntaxRule()
{
}

void SyntaxRule::addChildRule(QSharedPointer<SyntaxRule> rule)
{
	mChildRules.append(rule);
}

bool SyntaxRule::link(SyntaxDefinition* def)
{
	mDefinition = def;

	if (mAttribute.isEmpty())
		mAttributeLink = NULL;
	else
	{
		mAttributeLink = def->getItemData(mAttribute);
		if (!mAttributeLink)
		{
			qDebug() << "Failed to link attribute: " << mAttribute;
			return false;
		}
	}

	if (!def->linkContext(mContext, &mContextLink))
		return false;

	//	Link all children too
	foreach (QSharedPointer<SyntaxRule> rule, mChildRules)
		if (!rule->link(def))
			return false;

	//	Rule specific link-ups
	switch (mType)
	{
	case RegExpr:
		mRegExp = QRegExp(mString, mCaseInsensitive ? Qt::CaseInsensitive : Qt::CaseSensitive);
		break;

	case Keyword:
		mKeywordLink = def->getKeywordList(mString);
		if (!mKeywordLink)
		{
			qDebug() << "Failed to link keyword list: " << mString;
			return false;
		}
		break;

	default:break;
	}

	if (mCaseInsensitive)
	{
		mString = mString.toLower();
		mCharacterA = mCharacterA.toLower();
		mCharacterB = mCharacterB.toLower();
	}

	return true;
}

int SyntaxRule::match(const QString &string, int position)
{
	int match = 0;

	switch (mType)
	{
	case DetectChar:
		if (mCaseInsensitive)
			match = (string.at(position).toLower() == mCharacterA ? 1 : 0);
		else
			match = (string.at(position) == mCharacterA ? 1 : 0);
		break;

	case Detect2Chars:
		if (position < string.length() - 1)
		{
			if (mCaseInsensitive)
				match = (string.at(position).toLower() == mCharacterA && string.at(position + 1).toLower() == mCharacterB ? 2 : 0);
			else
				match = (string.at(position) == mCharacterA && string.at(position + 1) == mCharacterB ? 2 : 0);
		}
		break;

	case AnyChar:
		if (mString.contains(string.at(position)))
			match = 1;
		break;

	case StringDetect:
		if (Tools::compareSubstring(string, mString, position, mCaseInsensitive?Qt::CaseInsensitive:Qt::CaseSensitive))
			match = mString.length();
		break;

	case WordDetect:
		if (position == 0 || mDefinition->isDeliminator(string.at(position - 1)))
			if (position == string.length() - mString.length() || mDefinition->isDeliminator(string.at(position + mString.length())))
				if (Tools::compareSubstring(string, mString, position, mCaseInsensitive?Qt::CaseInsensitive:Qt::CaseSensitive))
					match = mString.length();
		break;

	case RegExpr:
	{
		int index = mRegExp.indexIn(string, position, QRegExp::CaretAtZero);
		if (index == position) match = mRegExp.matchedLength();
		break;
	}

	case IncludeRules:
		qDebug() << "Warning: IncludeRules left in the SyntaxDefinition after linking! :(";
		break;

	case Keyword:
	{
		foreach (QString keyword, mKeywordLink->items)
		{
			if (Tools::compareSubstring(string, keyword, position, mCaseInsensitive?Qt::CaseInsensitive:Qt::CaseSensitive))
			{
				if (position + keyword.length() >= string.length() || mDefinition->isDeliminator(string.at(position + keyword.length())))
					match = keyword.length();
				break;
			}
		}
		break;
	}

	case DetectSpaces:
		while (position + match < string.length() && string.at(position + match).isSpace())
			match++;
		break;

	case HlCOct:
		//	Octals start with 0, but have no x like hex. eg; 01562 is octal.
		if (string.at(position) == '0')
		{
			int lookahead = 1;
			const QChar* s = string.constData() + position + 1;
			while (position + lookahead < string.length() && (*s >= '0') && (*s <= '7'))
				s++,lookahead++;

			if (lookahead > 1)
				match = lookahead;
		}
		break;

	case Int:
	{
		//	Ints are any numbers 0-9
		const QChar* s = string.constData() + position;
		while (position + match < string.length() && (*s >= '0') && (*s <= '9'))
			s++,match++;
		break;
	}

	case DetectIdentifier:
	{
		//	[a-zA-Z_][a-zA-Z0-9_]*
		const QChar* s = string.constData() + position;
		if ((*s >= 'a' && *s <= 'z') || (*s >= 'A' &&  *s <= 'Z') || *s == '_')
		{
			s++;
			match = 1;
			while (position + match < string.length() && ((*s >= 'a' && *s <= 'z') || (*s >= 'A' &&  *s <= 'Z') || *s == '_' || (*s >= '0' && *s <= '9')))
				s++,match++;
		}
		break;
	}

	case Float:
	case HlCHex:
	case HlCStringChar:
	case HlCChar:
	case RangeDetect:
	case LineContinue:
		qDebug() << "Rule not honoured: " << mName;
		break;
	}

	return match;
}




























