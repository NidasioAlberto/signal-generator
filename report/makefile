
report.pdf: report.tex
	pdflatex $<  | tee latex.log | egrep "arning|eror"

watch: report.pdf
	open $<

clean:
	rm -f *.synctex.gz *.log *.aux
	rm -f *.bbl *.blg *.fls *.fdb_latexmk *.out *.pdf *.ps main.ppress.tex
	rm -rf auto _region_* submission
	rm -f *.zip
	rm -f *.cb2 *.cb
	rm -rf revision
