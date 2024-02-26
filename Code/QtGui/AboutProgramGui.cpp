#include "AboutProgramGui.hpp"

#include "BuildInfo.hpp"

#define VERSION_NUMBER "1.2.0"

#define STRINGIZE(x) #x
#define STRINGIZE2(x) STRINGIZE(x)

#define VERSION_STRING "Version: " VERSION_NUMBER "." STRINGIZE2(SM_CONVERTER_BUILD_VERSION)

struct LinkLabelData
{
	std::string_view text;
	std::string_view link;
};

const static std::vector<LinkLabelData> g_libraryLabelRows[] =
{
	{
		{ "LZ4", "https://github.com/lz4/lz4" },
		{ "Assimp", "https://github.com/assimp/assimp" }
	},
	{
		{ "GLM", "https://github.com/g-truc/glm" },
		{ "Valve VDF", "https://github.com/TinyTinni/ValveFileVDF" }
	},
	{
		{ "simdjson", "https://github.com/simdjson/simdjson" },
		{ "PerlinNoise", "https://github.com/Reputeless/PerlinNoise" }
	},
	{
		{ "FreeImage", "https://freeimage.sourceforge.io/" },
		{ "Nlohmann Json", "https://github.com/nlohmann/json" }
	}
};

RichTextLabel::RichTextLabel(const QString& text, QWidget* parent)
	: QLabel(text, parent)
{
	this->setTextFormat(Qt::RichText);
	this->setTextInteractionFlags(Qt::TextBrowserInteraction);
	this->setOpenExternalLinks(true);
}

RichTextLabel::RichTextLabel(const QString& text, const QString& link, QWidget* parent)
	: RichTextLabel(QString("<a href=\"%1\">%2</a>").arg(link).arg(text), parent) {}

///////////////ABOUT PROGRAM GUI//////////////////////

const char* g_programDescription = "SM Converter allows you to convert blueprints, tiles and worlds from Scrap Mechanic.";

AboutProgramGui::AboutProgramGui(QWidget* parent)
	: QDialog(parent),
	m_description(new QLabel(g_programDescription, this)),
	m_mainLayout(new QVBoxLayout(this)),
	m_subLayout(new QHBoxLayout(this)),

	m_authorsPanelLayout(new QVBoxLayout(this)),
	m_versionLabel(new QLabel(VERSION_STRING, this)),
	m_authorsLabel(new QLabel("Created by:", this)),
	m_author1Label(new RichTextLabel("<a href=\"https://github.com/QuestionableM\">Questionable Mark</a>", this)),
	m_author2Label(new RichTextLabel("<a href=\"https://github.com/Kariaro\">Kariaro (Hardcoded)</a>", this)),

	m_librariesLayout(new QVBoxLayout(this))
{
	this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	this->setWindowTitle("About Program");
	m_description->setWordWrap(true);

	m_mainLayout->setSpacing(40);
	m_mainLayout->addWidget(m_description, 0, Qt::AlignTop);
	m_mainLayout->addLayout(m_subLayout);

	m_subLayout->setContentsMargins(0, 0, 0, 0);
	m_subLayout->setSpacing(60);
	m_subLayout->addLayout(m_authorsPanelLayout, 0);
	m_subLayout->addLayout(m_librariesLayout);

	m_authorsPanelLayout->setAlignment(Qt::AlignBottom);
	m_authorsPanelLayout->setContentsMargins(0, 0, 0, 0);
	m_authorsPanelLayout->setSpacing(2);
	m_authorsPanelLayout->addWidget(m_versionLabel);
	m_authorsPanelLayout->addWidget(m_authorsLabel);
	m_authorsPanelLayout->addWidget(m_author1Label);
	m_authorsPanelLayout->addWidget(m_author2Label);

	m_librariesLayout->setAlignment(Qt::AlignBottom);
	m_librariesLayout->setContentsMargins(0, 0, 0, 0);
	m_librariesLayout->setSpacing(2);

	constexpr std::size_t g_numLibraryRows = sizeof(g_libraryLabelRows)
		/ sizeof(std::remove_all_extents_t<decltype(g_libraryLabelRows)>);

	for (std::size_t a = 0; a < g_numLibraryRows; a++)
	{
		QHBoxLayout* v_new_row = new QHBoxLayout(this);
		v_new_row->setAlignment(Qt::AlignRight | Qt::AlignBottom);
		v_new_row->setSpacing(10);

		for (const auto& v_cur_label : g_libraryLabelRows[a])
			v_new_row->addWidget(new RichTextLabel(
				QString::fromLatin1(v_cur_label.text.data(), v_cur_label.text.size()),
				QString::fromLatin1(v_cur_label.link.data(), v_cur_label.link.size()),
				this));

		m_librariesLayout->addLayout(v_new_row);
	}

	this->adjustSize();
}