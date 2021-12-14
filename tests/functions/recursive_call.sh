test()
{
    if echo $@; then
        test false
    else
        test true
    fi
    echo end
}

test false
echo "did this work ?"
