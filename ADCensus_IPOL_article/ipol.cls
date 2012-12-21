%-------------------------------------------------------------------------------
% ipol.cls
% LaTeX class for IPOL articles
%
% version 0.2 - 2012.6.4 - rafael grompone von gioi
%                           > the logo image is now called "ipol_logo".
%                           > affiliations command added
%                           > "DRAFT" label became "PREPRINT"
%                           > margins copied from "fullpage" without using it
%                           > "\ipolDraftLink" command became
%                             "\ipolPreprintLink"
%                           > modifications to the paper's header
%                           > "\href" command was modified in order that links
%                             will include a footnote with the address, to get
%                             the links in printed form too.
%                           > links' color modified.
%                           > compilation date added to the Preprint label
%                           > modifications to the mechainsm that generates
%                             the title of the article.
% version 0.1 - 2012.1.27 - rafael grompone von gioi
%-------------------------------------------------------------------------------
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{ipol}
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}
\ProcessOptions
\LoadClass[a4paper,12pt]{article}

% Margins
% values copied from "fullpage" package usign "cm" option
\setlength{\headheight}{0pt}
\setlength{\headsep}{0pt}
\setlength{\textwidth}{\paperwidth}
\addtolength{\textwidth}{-3cm}
\setlength{\oddsidemargin}{1.5cm}
\addtolength{\oddsidemargin}{-1in}
\setlength{\evensidemargin}{\oddsidemargin}
\setlength{\textheight}{\paperheight}
\addtolength{\textheight}{-\headheight}
\addtolength{\textheight}{-\headsep}
\addtolength{\textheight}{-\footskip}
\addtolength{\textheight}{-3cm}
\setlength{\topmargin}{1.5cm}
\addtolength{\topmargin}{-1in}

% Links package and colors definition
\RequirePackage{color}
\definecolor{link}{rgb}{0.240,0.194,0.431}
\RequirePackage[colorlinks,
                linkcolor=link,
                anchorcolor=link,
                citecolor=link,
                filecolor=link,
                menucolor=link,
                pagecolor=link,
                urlcolor=link]{hyperref}
% The following definition modify the "\href" command to
% also include a footnote with the address, so it is also
% accessible in printed form. The command "\basehref" is
% created in cases where the footnote wants to be avoided.
\newcommand*\basehref[2]{}
\let\basehref\href
\renewcommand*\href[2]{\basehref{#1}{#2}\footnote{\url{#1}}}

% IPOL environments
\renewenvironment{abstract}{\small%
                            \begin{center}%
                            {\bf Abstract\vspace{-.5em}}%
                            \end{center}%
                            \begin{quote}%
                            \noindent\ignorespaces}{\end{quote}}
\newenvironment{ipolCode}{\small%
                          \begin{center}%
                          {\bf Source Code\vspace{-.5em}}%
                          \end{center}%
                          \begin{quote}%
                          \noindent\ignorespaces}{\end{quote}}
\newenvironment{ipolSupp}{\small%
                          \begin{center}%
                          {\bf Supplementary Material\vspace{-.5em}}%
                          \end{center}%
                          \begin{quote}%
                          \noindent\ignorespaces}{\end{quote}}

% IPOL data
\newcommand{\ipolTitle}{title}
\newcommand{\ipolAuthors}{authors}
\newcommand{\ipolAffiliations}{}
\newcommand{\ipolYear}{YYYY}
\newcommand{\ipolMonth}{MM}
\newcommand{\ipolDay}{DD}
\newcommand{\ipolID}{XXXXXXXX}
\newcommand{\ipolDOI}{10.5201/ipol.\ipolYear.\ipolID}
\newcommand{\ipolLink}{http://dx.doi.org/\ipolDOI}

% IPOL data set commands
\newcommand{\ipolSetTitle}[1]{\renewcommand{\ipolTitle}{#1}%
                              \newcommand{\ipolTitleDefined}{}}
\newcommand{\ipolSetAuthors}[1]{\renewcommand{\ipolAuthors}{#1}%
                                \newcommand{\ipolAuthorsDefined}{}}
\newcommand{\ipolSetAffiliations}[1]{\renewcommand{\ipolAffiliations}{#1}%
                                \newcommand{\ipolAffiliationsDefined}{}}
\newcommand{\ipolSetYear}[1]{\renewcommand{\ipolYear}{#1}%
                             \newcommand{\ipolYearDefined}{}}
\newcommand{\ipolSetMonth}[1]{\renewcommand{\ipolMonth}{#1}%
                              \newcommand{\ipolMonthDefined}{}}
\newcommand{\ipolSetDay}[1]{\renewcommand{\ipolDay}{#1}%
                            \newcommand{\ipolDayDefined}{}}
\newcommand{\ipolSetID}[1]{\renewcommand{\ipolID}{#1}%
                           \newcommand{\ipolIDDefined}{}}
\newcommand{\ipolPreprintLink}[1]{\renewcommand{\ipolLink}{#1}%
                                  \newcommand{\ipolPreprint}{}}

% Print IPOL Header and Title
\RequirePackage{graphicx}
\AtBeginDocument{%
% Decide if it works in Normal or Preprint mode. This must be done at
% the beginning of the document, when all IPOL definitions were already done.
\@ifundefined{ipolTitleDefined}{\newcommand{\ipolIsPreprint}{}}{%
  \@ifundefined{ipolAuthorsDefined}{\newcommand{\ipolIsPreprint}{}}{%
    \@ifundefined{ipolYearDefined}{\newcommand{\ipolIsPreprint}{}}{%
      \@ifundefined{ipolMonthDefined}{\newcommand{\ipolIsPreprint}{}}{%
        \@ifundefined{ipolDayDefined}{\newcommand{\ipolIsPreprint}{}}{%
          \@ifundefined{ipolIDDefined}{\newcommand{\ipolIsPreprint}{}}{%
            \@ifundefined{ipolPreprint}{}{\newcommand{\ipolIsPreprint}{}}}}}}}}
\includegraphics[width=.27\linewidth]{ipol_logo}%
\hspace{1em}%
\vbox{\scriptsize\sf\noindent%
Published in \basehref{http://dx.doi.org/10.5201/ipol}{Image Processing On Line}
on \ipolYear--\ipolMonth--\ipolDay.\\
ISSN 2105--1232 \copyright\ \ipolYear\ IPOL \& the authors
\basehref{http://creativecommons.org/licenses/by-nc-sa/3.0/}{CC--BY--NC--SA}\\
This article is available online with supplementary materials,\\
software, datasets and online demo at\\
\url{\ipolLink}}
\smallskip
\@ifundefined{ipolIsPreprint}{}{%
  \begin{center}%
  {\sf\Large\color{red}PREPRINT \@date}%
  \end{center}%
}
\begin{center}
\Large\textbf{\ipolTitle}\\
\bigskip
\large\ipolAuthors\\
\bigskip
\@ifundefined{ipolAffiliationsDefined}{}{%
  \footnotesize\ipolAffiliations\\%
  \bigskip%
}
\end{center}
}% end of \AtBeginDocument

%-------------------------------------------------------------------------------
