# Set titles and labels
set title "Average Time vs Number of Tellers"
set xlabel "Number of Tellers"
set ylabel "Average Time in Bank (minutes)"
set grid

# Set output to PNG
set terminal pngcairo size 800,600 enhanced font 'Arial,12'
set output "output/plot.png"

# Use histogram style for bars
set style data histograms
set style fill solid 1.0 border -1       # solid fill with border
set boxwidth 0.5                          # width of bars

# Plot data from file
# Assuming your plot_data.txt has two columns: teller_number average_time
plot "output/plot_data.txt" using 2:xtic(1) title "Average Time"

unset output
