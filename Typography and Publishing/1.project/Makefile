NAME=proj1

$(NAME).pdf: $(NAME).ps
	ps2pdf $(NAME).ps

$(NAME).ps: $(NAME).dvi
	dvips -t a4 $(NAME).dvi

$(NAME).dvi: $(NAME).tex 
	latex $(NAME).tex
	latex $(NAME).tex

.PHONY: 
	clean pack
clean:
	rm -f $(NAME).dvi $(NAME).aux $(NAME).toc $(NAME).log $(NAME).out $(NAME).ps

pack:
	tar -cvzf xstupi00-fit.tar.gz $(NAME).tex makefile
