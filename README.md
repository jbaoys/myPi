# myPi
Play SBC such as RaspberryPi with sorts of ideas


# git config

Configuration & set up: git config
Once you have a remote repo setup, you will need to add a remote repo url to your local git config, and set an upstream branch for your local branches. The git remote command offers such utility.

git remote add <remote_name> <remote_repo_url>

This command will map remote repository at <remote_repo_url> to a ref in your local repo under <remote_name>. Once you have mapped the remote repo you can push local branches to it.

git push -u <remote_name> <local_branch_name>

This command will push the local repo branch under <local_branc_name> to the remote repo at <remote_name>.

For more in-depth look at git remote, see the Git remote page.

In addition to configuring a remote repo URL, you may also need to set global Git configuration options such as username, or email. The git config command lets you configure your Git installation (or an individual repository) from the command line. This command can define everything from user info, to preferences, to the behavior of a repository. Several common configuration options are listed below.

Git stores configuration options in three separate files, which lets you scope options to individual repositories (local), user (Global), or the entire system (system):

Local: <repo>/.git/config – Repository-specific settings.
Global: /.gitconfig – User-specific settings. This is where options set with the --global flag are stored.
System: $(prefix)/etc/gitconfig – System-wide settings.

Define the author name to be used for all commits in the current repository. Typically, you’ll want to use the --global flag to set configuration options for the current user.

git config --global user.name <name>

Define the author name to be used for all commits by the current user.

Adding the --local option or not passing a config level option at all, will set the user.name for the current local repository.

git config --local user.email <email>

Define the author email to be used for all commits by the current user.

git config --global alias.<alias-name> <git-command>

Create a shortcut for a Git command. This is a powerful utility to create custom shortcuts for commonly used git commands. A simplistic example would be:

git config --global alias.ci commit

This creates a ci command that you can execute as a shortcut to git commit. To learn more about git aliases visit the git config page.

git config --system core.editor <editor>

Define the text editor used by commands like git commit for all users on the current machine. The <editor> argument should be the command that launches the desired editor (e.g., vi). This example introduces the --system option. The --system option will set the configuration for the entire system, meaning all users and repos on a machine. For more detailed information on configuration levels visit the git config page.

git config --global --edit

Open the global configuration file in a text editor for manual editing. An in-depth guide on how to configure a text editor for git to use can be found on the Git config page.

# Discussion

All configuration options are stored in plaintext files, so the git config command is really just a convenient command-line interface. Typically, you’ll only need to configure a Git installation the first time you start working on a new development machine, and for virtually all cases, you'll want to use the --global flag. One important exception is to override the author email address. You may wish to set your personal email address for personal and open source repositories, and your professional email address for work-related repositories.

Git stores configuration options in three separate files, which lets you scope options to individual repositories, users, or the entire system:

<repo>/.git/config – Repository-specific settings.

~/.gitconfig – User-specific settings. This is where options set with the --global flag are stored.
$(prefix)/etc/gitconfig – System-wide settings.
When options in these files conflict, local settings override user settings, which override system-wide. If you open any of these files, you’ll see something like the following:

[user] name = John Smith email = john@example.com [alias] st = status co = checkout br = branch up = rebase ci = commit [core] editor = vim
You can manually edit these values to the exact same effect as git config.

Example

The first thing you’ll want to do after installing Git is tell it your name/email and customize some of the default settings. A typical initial configuration might look something like the following:

Tell Git who you are git config

git --global user.name "John Smith" git config --global user.email john@example.com

Select your favorite text editor

git config --global core.editor vim

Add some SVN-like aliases

git config --global alias.st status
git config --global alias.co checkout
git config --global alias.br branch
git config --global alias.up rebase
git config --global alias.ci commit

This will produce the ~ /.gitconfig file from the previous section. Take a more in-depth look at git config on the git config page.

Using Token to push
===================
exmaple:
$ git push https://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@github.com/baojason/myPi.git

