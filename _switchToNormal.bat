@ECHO OFF
call _cleanup.bat

del qtc_packaging\debian_harmattan\control
copy qtc_packaging\debian_harmattan\control_normal qtc_packaging\debian_harmattan\control

del qtc_packaging\debian_harmattan\rules
copy qtc_packaging\debian_harmattan\rules_normal qtc_packaging\debian_harmattan\rules

del qtc_packaging\debian_harmattan\changelog
copy qtc_packaging\debian_harmattan\changelog_normal qtc_packaging\debian_harmattan\changelog

