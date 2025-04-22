TEMPLATE = subdirs

SUBDIRS += \
    MeasurementTest \
    DataAnalyzerTest \
    DatabaseHandlerTest

MeasurementTest.depends = 
DataAnalyzerTest.depends = 
DatabaseHandlerTest.depends = 

CONFIG += ordered