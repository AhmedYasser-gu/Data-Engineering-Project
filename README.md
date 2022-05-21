# Team 11 Lightning McQueen


##### DIT638 Cyber physical Systems and Systems of Systems
---
## How to get and build the project

#### **Tools required (prior):**

[Linux installation **(if not your main OS then use Virtual box)**](https://www.virtualbox.org/)

[Setting up Ubuntu](https://itsfoss.com/install-ubuntu-1404-dual-boot-mode-windows-8-81-uefi/)

[Git](https://www.atlassian.com/git/tutorials/install-git#linux) 

[Docker](https://dockersl.im/)

Cmake, Make and a C++ compiler, which can be installed with these packages:  
 
    sudo apt-get update
    sudo apt-get install build-essential cmake git

---
- First you must clone this repository into a folder on your computer. Navigate to the folder you want to clone the repo to in your terminal then run the following command.
 
- Inside the folder you cloned the repo into there will now be a folder called **2022-group-11**. Move into this folder.


Move into the **src** folder

    cd src

To build the project with Docker run this command, and it will create a Docker image:

    docker build -t NAME-OF-CHOICE -f Dockerfile .

To build with Cmake, make a build directory (named "build" so the resulting binaries are ignored by git), move into the directory then call cmake and make on the source files.
In sequence the commands to run are as follows:

    mkdir build
    cd build
    cmake ..
    make

# Way of working

## Adding features and resolving issues

Our team intends to work with a regularly updated main branch, with short-lived quickly integrated feature branches, consisting of increments of work that can be finished in a day or two.

The features to be added will be decided on during Sprint meetings at the beginning of each week. Additions to the system that occur during the work week that were not discussed during the Sprint meeting will be communicated to the rest of the team, to keep everyone up to date.

The team will be in regular communication, and if there are issues a team member can call upon other members of the team to review their code for faults or otherwise work through the issue until it is resolved. 

Code review can happen in person, over Discord, or through the Gitlab website code review feature. 

We will also make use of the Issue tag system offered by Gitlab, to make current issues in the code clear to the team, and to show when they have been resolved and by what commit. 

Our team has agreed to write unit tests for their own code, so that wnever a testable piece of logic has been written, there should also be a unit test to ensure it functions properly.

We will integrate the use of some tools to enforce code quality and discover issues with the code.

[**Trello**](https://trello.com/invite/b/kwqC7L23/d5d8160708adcd3f1ea1f9c45477959c/dit638-group11) will act as our Kanban board for general project management.

### Tools:
**scan-build** for code linting  

**clang-tidy** also for code linting  

**vera++** for code formatting  

**Catch2** for unit tests

**OpenDLV** Mircoservice-based software to support autonomous part of the vehicle

## Commit style guide

Attempt to limit commit messages to 50 characters or less

Capitalize the subject line eg. “Make commit”

Do not end subject line with a period, to save space

Use imperative mood eg. “Make commit”, “Run down hill”. All commit messages should finish this sentence:
> If applied, this commit will X

Commit messages should communicate what the commit is and why it is being done instead of how.

We will not initially use multi line commit messages, that is; commit messages with a heading and a body. We might incorporate this in the future.



## Developers
- [Adam Telles](https://git.chalmers.se/adamte)
- [Ahmed Yasser](https://git.chalmers.se/ahmedya)
- [Alex Nordin](https://git.chalmers.se/lnoklal)
- [Axel Broberg](https://git.chalmers.se/axelbro)
- [Niklas Höglund](https://git.chalmers.se/nikhog)
