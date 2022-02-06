### Project description

```c
/*==================================================*/
// Task 2. Propose a housekeeping tool: to remove files based on maximum age (days), or maximum number to keep (using C programming language).
//
// support two configuration, maximum age in days, or maximum number of files to keep
// maximum age: remove all files that are older than X days, maximum number of files: remove oldest files if the number of files are more than X files.
// support yaml config file format to specify the directories and configurations
//         GUI is not necessary, cli program with a YAML config file to read
/*==================================================*/
```

### Demo image
<img src="./img/demo-cuddly-yaml.gif">

### Prerequisite

1. [glib](https://github.com/GNOME/glib)  
2. [libcyaml](https://github.com/tlsa/libcyaml)

### Compile the source code and clean

```bash
# compile
make 

# clean
make clean 
```

### Getting started

1. Set the desired value in `config.yaml` in this directory

```yaml
    max_age_in_days:
      days: -1 # means delete all
``` 
1. Run the following command
```bash
./cuddly-yaml ./config.yaml

# If you need to access directory with permission, kindly use
sudo ./cuddly-yaml ./config.yaml
```

### Tested Enviornment
    [x] MacOS 




# Reference

[glib](https://developer.ibm.com/tutorials/l-glib/)

[C program to delete file](https://www.geeksforgeeks.org/c-program-delete-file/)

[glib-example](https://github.com/steshaw/gtk-examples)

[Check file date in c](https://c-for-dummies.com/blog/?p=3004)

[Check file creation date in c](https://stackoverflow.com/questions/5929419/how-to-get-file-creation-date-in-linux)

[Time and return value](https://www.ibm.com/docs/en/i/7.4?topic=functions-ctime-r-convert-time-character-string-restartable)

[glib gstring](https://gist.github.com/johnkawakami/3551598)
