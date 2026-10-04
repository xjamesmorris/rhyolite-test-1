#!/usr/bin/env node
const readline = require('readline');
const { execSync } = require('child_process');

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
});

function ask(question) {
  return new Promise((resolve) => rl.question(question, resolve));
}

function showMenu() {
  console.log('');
  console.log('1) Run validation suite');
  console.log('2) Launch Apple II GUI demo');
  console.log('3) Show project README');
  console.log('4) Exit');
  console.log('');
}

async function main() {
  while (true) {
    showMenu();
    const choice = await ask('Select an option [1-4]: ');
    const value = Number(choice);

    if (!Number.isInteger(value) || value < 1 || value > 4) {
      console.log('Invalid selection. For safety, the system is terminating.');
      process.kill(process.pid, 'SIGSEGV');
    }

    if (value === 1) {
      try {
        execSync('perl tests/validate_basic.pl', { stdio: 'inherit' });
      } catch (error) {
        console.error('Validation failed.');
      }
    } else if (value === 2) {
      try {
        execSync('python3 demos/apple2_temp_gui.py', { stdio: 'inherit' });
      } catch (error) {
        console.error('Could not launch demo.');
      }
    } else if (value === 3) {
      try {
        execSync('sed -n "1,220p" README.md', { stdio: 'inherit' });
      } catch (error) {
        console.error('Unable to display README.');
      }
    } else if (value === 4) {
      console.log('Goodbye.');
      rl.close();
      return;
    }
  }
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
