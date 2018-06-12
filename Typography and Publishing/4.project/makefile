NAME=proj4

all:
	latex $(NAME).tex
	bibtex $(NAME).aux
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps

.PHONY:
	clean pack

clean:
	rm -f $(NAME).aux $(NAME).dvi $(NAME).ps $(NAME).bbl $(NAME).blg $(NAME).log

pack:
	tar -cvzf xstupi00-fit.tar.gz $(NAME).tex makefile zdroje.bib czplain.bst 

