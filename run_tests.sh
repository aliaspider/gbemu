
testroms_dir=testroms

cd $testroms_dir

for testrom in *.gb ; do
   if (../gbemu_test "$testrom" --silent ) then
      echo "test succeeded : $testrom"
   else
      echo "test failed    : $testrom"
   fi

done

cd ..
