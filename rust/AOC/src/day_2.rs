use std::io::{BufRead, BufReader};
use std::fs::File;

pub fn day2() -> Result<(),anyhow::Error>{
    let f = File::open("input2.txt")?;

    let reader = BufReader::new(f);


    println!("FILE has been opened");
    let mut count = 0;
    let mut lines = reader.lines();
    while let Some(Ok(line)) = lines.next(){
        let mut content = line.split(" ");
        
        let mut values = vec![];
        while let Some(v) = content.next(){
            values.push(v);
        }

        let values_int: Vec<i32> = values
        .iter()
        .filter_map(|s| s.parse::<i32>().ok())
        .collect();

        if is_valid(values_int) {
            count+=1;
        }


    }

    println!("The total safe count is {}",count);
    Ok(())
}

fn is_valid(v: Vec<i32>) -> bool {
    if v.len() < 2 {
        return false;
    }

    let diff = v[1] - v[0];
    if diff == 0 || diff < -3 || diff > 3 {
        return false;
    }

    let increasing = diff > 0;

    for i in 1..v.len() {
        let step = v[i] - v[i-1];

        if increasing {
            if step < 1 || step > 3 {
                return false;
            }
        } else {
            if step > -1 || step < -3 {
                return false;
            }
        }
    }

    return true;
}