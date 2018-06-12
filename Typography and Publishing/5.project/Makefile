NAME=proj5

all:
	pdflatex $(NAME).tex
	bibtex $(NAME).aux
	pdflatex $(NAME).tex
	pdflatex $(NAME).tex
.PHONY:
	clean pack 

clean:
	rm -f $(NAME).aux $(NAME).dvi $(NAME).ps $(NAME).bbl $(NAME).blg $(NAME).log  $(NAME).nav $(NAME).out $(NAME).snm $(NAME).toc

pack:
	tar -cvzf xstupi00-fit.tar.gz $(NAME).tex Makefile references.bib logo.png logo1.png  
