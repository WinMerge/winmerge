Some useful scripts:

changelog.py
------------
 Python script creating a changelog from SVN. The script reads a commit log
 from SVN and outputs formatted changelog.

 Usage: python changelog.py [-h] [--help] [-r:n] [--revisions:n]
  Where:
   -h, --help print usage help
   -r:n, --revisions:n output log for last n revisions (default is 100)
