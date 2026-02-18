import streamlit as st
import pandas as pd
import plotly.express as px
import time

# ---------------------------------------------------------
# 1. Page Configuration
# ---------------------------------------------------------
st.set_page_config(layout="wide", page_title="Professional Task Manager")

st.title("🖥️ System Resource Monitor (Live)")

# Create layout: Top Row (Key Metrics), Middle Row (Charts)
metric_col1, metric_col2, metric_col3, metric_col4 = st.columns(4)
chart_col1, chart_col2 = st.columns(2)

# ---------------------------------------------------------
# 2. Data Loading & Formatting Function
# ---------------------------------------------------------
def load_data():
    try:
        df = pd.read_csv("../agent/output/system_logs.csv")
        
        # Safety Check
        if 'RAM_Used_MB' not in df.columns:
            return None
            
        # Sort by timestamp to ensure order
        df = df.sort_values('Timestamp')

        # --- THE FIX: Calculate Relative Time ---
        # 1. Get the start time (first row)
        start_time = df['Timestamp'].iloc[0]
        
        # 2. Subtract start time from all rows to get 0, 1, 2, 3...
        df['Seconds_Running'] = df['Timestamp'] - start_time
        
        # Keep the last 60 records so the chart scrolls
        return df.tail(60)
    except Exception:
        return None
# ---------------------------------------------------------
# 3. Main Display Loop
# ---------------------------------------------------------
# Load the latest data
data = load_data()

if data is not None and not data.empty:
    latest = data.iloc[-1]
    
    # --- TOP METRICS ROW ---
    # CPU: Sum of User + Kernel to show total load
    total_cpu = latest['CPU_User'] + latest['CPU_Kernel']
    metric_col1.metric("CPU Usage", f"{total_cpu:.1f}%")
    
    # RAM: Show usage in GB (divide MB by 1024)
    ram_used_gb = latest['RAM_Used_MB'] / 1024
    ram_total_gb = latest['RAM_Total_MB'] / 1024
    metric_col2.metric("RAM Usage", f"{ram_used_gb:.2f} GB", f"of {ram_total_gb:.2f} GB")
    
    # Process & Thread Counts
    metric_col3.metric("Processes", int(latest['Process_Count']))
    metric_col4.metric("Active Threads", int(latest['Thread_Count']))

    # --- LEFT CHART: CPU PIE DISTRIBUTION ---
    # --- LEFT CHART: CPU PIE DISTRIBUTION ---
    with chart_col1:
        st.subheader("CPU Distribution")
        
        # We explicitly map names to specific hex colors
        # User = Green, Kernel = Red, Idle = Blue/Grey
        fixed_colors = {
            'User': '#00CC96',   # Green
            'Kernel': '#EF553B', # Red
            'Idle': '#636EFA'    # Blue
        }
        
        fig_cpu = px.pie(
            names=['User', 'Kernel', 'Idle'],
            values=[latest['CPU_User'], latest['CPU_Kernel'], latest['CPU_Idle']],
            hole=0.5,
            color=['User', 'Kernel', 'Idle'], # Tells Plotly which column drives color
            color_discrete_map=fixed_colors   # Forces the specific color mapping
        )
        
        # This ensures the legend stays sorted so colors don't swap positions
        fig_cpu.update_traces(sort=False) 
        
        st.plotly_chart(fig_cpu, use_container_width=True)

    # --- RIGHT CHART: RAM HISTORY (READABLE TIME) ---
   # --- RIGHT CHART: RAM HISTORY (RELATIVE TIME) ---
    with chart_col2:
        st.subheader("RAM Usage History")
        
        # Use 'Seconds_Running' for X-axis
        fig_ram = px.area(data, x='Seconds_Running', y='RAM_Used_MB', 
                          range_y=[0, latest['RAM_Total_MB']],
                          labels={'RAM_Used_MB': 'RAM (MB)', 'Seconds_Running': 'Time (Seconds Running)'})
        
        # Clean up the chart look
        fig_ram.update_layout(xaxis_title="Seconds Since Start")
        st.plotly_chart(fig_ram, use_container_width=True)

else:
    # This shows if the CSV is empty or missing headers
    st.info("Waiting for Agent to generate data...")

# ---------------------------------------------------------
# 4. Refresh Logic
# ---------------------------------------------------------
time.sleep(1)
st.rerun()