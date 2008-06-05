/*! \file
    \brief Header of the html_builder_t

- This header aims to ease the construction of a html page.
- the whole class is defined in the header .hpp

*/


#ifndef __NEOIP_HTML_BUILDER_HPP__ 
#define __NEOIP_HTML_BUILDER_HPP__ 
/* system include */
#include <string>

/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief This class helps building html
 */
class html_builder_t {
public:
	/*************** address tag	***************************************/
	std::string s_link(std::string s, const std::string &attr = "")
							const { return "<a href=\"" + s + "\" " + attr  +">";	}
	std::string e_link()				const { return "</a>";	}

	/*************** br tag	***********************************************/
	std::string br(const std::string &attr = "")	const { return "<br " + attr + ">";	}

	/*************** hr tag	***********************************************/
	std::string hr(const std::string &attr = "")	const { return "<hr " + attr + ">";	}

	/*************** bold tag	***************************************/
	std::string s_b(const std::string &attr = "")	const { return "<b " + attr + ">";	}
	std::string e_b()				const { return "</b>";			}
	std::string b(const std::string &s)		const { return this->s_b() + s + this->e_b(); }

	/*************** preformated tag	*******************************/
	std::string s_pre(const std::string &attr = "")	const { return "<pre " + attr + ">";	}
	std::string e_pre()				const { return "</pre>";	}
	std::string pre(const std::string &s)		const { return this->s_pre() + s + this->e_pre();}
	
	/*************** preformated tag	*******************************/
	std::string s_caption(const std::string &attr = "")	const { return "<caption " + attr + ">";	}
	std::string e_caption()				const { return "</caption>";	}
	std::string caption(const std::string &s)	const { return this->s_caption() + s + this->e_caption();}

	/*************** table row tag	***************************************/
	std::string s_ol(const std::string &attr = "")	const { return "<ol " + attr + ">";	}
	std::string e_ol()				const { return "</ol>";			}
	std::string s_ul(const std::string &attr = "")	const { return "<ul " + attr + ">";	}
	std::string e_ul()				const { return "</ul>";			}
	std::string s_li(const std::string &attr = "")	const { return "<li " + attr + ">";	}
	std::string e_li()				const { return "</li>";			}

	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	//                    TABLE
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	
	/*************** table tag	***************************************/
	std::string s_table(const std::string &attr = "cellspacing=\"0\" cellpadding=\"2\" border=\"1\" width=\"100\%\"")
							const { return "<table " + attr + ">";	}
	std::string e_table()				const { return "</table>";		}

	/*************** table header tag	*******************************/
	std::string s_th(const std::string &attr = "") 	const { return "<td " + attr + ">";	}
	std::string e_th()				const { return "</th>";			}
	std::string th(const std::string &s)		const { return this->s_th() + s + this->e_th(); }
	
	/*************** table row tag	***************************************/
	std::string s_tr(const std::string &attr = "")	const { return "<tr " + attr + ">";	}
	std::string e_tr()				const { return "</tr>";			}
	std::string tr(const std::string &s)		const { return this->s_tr() + s + this->e_tr(); }

	/*************** table data tag	***************************************/
	std::string s_td(const std::string &attr = "")	const { return "<td " + attr + ">";	}
	std::string e_td()				const { return "</td>";			}
	std::string td(const std::string &s)		const { return this->s_td() + s + this->e_td(); }

	//!< helper for backward compatibility - depreciated
	std::string s_table_packed_noborder()		const {	return s_table("");		}
	std::string s_td_title(const std::string &title_str)
							const { return s_td("title=\"" + title_str + "\"");	}
	std::string s_th_title(const std::string &title_str)
							const { return s_th("title=\"" + title_str + "\"");	}


	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	//                    Some custom stuff - kind of lame css
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	std::string s_pagetitle()			const { return "<h1><div align=\"center\">";	}
	std::string e_pagetitle()			const { return "</div></h1>";	}
	std::string pagetitle(const std::string &s)	const { return this->s_pagetitle() + s + this->e_pagetitle(); }

	std::string s_sub1title()			const { return "<hr><font size=\"+1\"><b>";	}
	std::string e_sub1title()			const { return "</b></font>";	}
	std::string sub1title(std::string s)		const { return this->s_sub1title() + s + this->e_sub1title(); }
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTML_BUILDER_HPP__  */



