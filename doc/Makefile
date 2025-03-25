# Target to generate documentation using Doxygen

default: all

all: doc pdf

doc:
	@if [ ! -f Doxyfile ]; then \
		echo "Doxyfile not found. Generating default Doxyfile..."; \
		doxygen -g; \
	fi
	
	@echo "Generating documentation with Doxygen..."
	doxygen

pdf:
	@echo "Generating PDF Manual..."
	cd latex && make pdf && cd ..
	mv latex/refman.pdf ./ifcc_doc.pdf
	rm -rf latex

clean: clean_html clean_pdf


clean_html:
	@echo "Cleaning html folder"
	rm -rf html

clean_pdf :
	@echo "Remove pdf"
	rm -f ifcc_doc.pdf
